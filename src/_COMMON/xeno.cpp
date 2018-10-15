#include <QDebug>
#include <QException>
#include <QDataStream>
#include <QVariant>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include "xeno.h"

// сериализация Xeno::Move::Snap (перегружаем оператор <<)
QDataStream &operator <<(QDataStream &in, Xeno::Move::Snap const &x)
{
	return in << x.B << x.W << x.S;
}

// десериализация Xeno::Move::Snap (перегружаем оператор >>)
QDataStream &operator >>(QDataStream &out, Xeno::Move::Snap &x)
{
	return out >> x.B >> x.W >> x.S;
}

Xeno::Xeno()
{
}

Xeno::~Xeno()
{
	_db.close();
}

bool Xeno::OpenDB(QString path)
{
	_db = QSqlDatabase::addDatabase("QSQLITE");
	_db.setDatabaseName(path);
	if (_db.open()) {
		QSqlQuery query(_db);
		query.prepare("PRAGMA synchronous = OFF;");
		query.exec();
	}
	return _db.isOpen();
}

uint32_t Xeno::AddPlayer(Player const & player)
{
	QSqlQuery query(_db);
	_db.transaction();
	query.prepare("INSERT INTO `Players` (`name`) VALUES (?);");
	query.addBindValue(player.name);
	if (query.exec()) {
		query.prepare("SELECT last_insert_rowid();");
		query.exec();
		_db.commit();
		query.next();
		uint32_t id = query.value(0).toUInt();
		return id;
	}
	_db.rollback();
	return 0;
}

uint32_t Xeno::AddParty(Party const & party)
{
	QSqlQuery query(_db);
	query.prepare(
				"INSERT INTO `Parties` (`playerIDb`, `playerIDw`, `dateStart`) "
				"VALUES (?, ?, ?);"
				  );
	query.addBindValue(party.playerIDb);
	query.addBindValue(party.playerIDw);
	query.addBindValue(party.dateStart.toSecsSinceEpoch());
	_db.transaction();
	if (query.exec()) {
		query.prepare("SELECT last_insert_rowid();");
		query.exec();
		_db.commit();
		query.next();
		return query.value(0).toUInt();
	}
	_db.rollback();
	return 0;
}

bool Xeno::AddMove(Party const & party, const Move & move)
{
	QByteArray arr;
	QDataStream stream(&arr, QIODevice::WriteOnly);
	stream << move.snap;
	QSqlQuery query(_db);
	query.prepare(
				"INSERT INTO `Moves` "
				"(`partyID`, `ordinal`, `snap`, `notation`, `player`, `strike`) "
				"VALUES (?, ?, ?, ?, ?, ?);"
				  );
	query.addBindValue(party._partyID);
	query.addBindValue(move.ordinal);
	query.addBindValue(arr);
	query.addBindValue(move.notation);
	query.addBindValue(move.currentPlayer);
	query.addBindValue(move.strike);
	_db.transaction();
	if (query.exec()) {
		_db.commit();
		return true;
	};
	qDebug()<<query.lastError();
	_db.rollback();
	return false;
}

bool Xeno::UpdateParty(const Xeno::Party & party)
{
	QSqlQuery query(_db);
	query.prepare(
				"UPDATE `Parties` SET "
				"`dateFinish` = ? "
				"WHERE `_partyID` = ?;"
				);
	query.addBindValue(party.dateFinish.toSecsSinceEpoch());
	query.addBindValue(party._partyID);
	_db.transaction();
	if (query.exec()) {
		_db.commit();
		return true;
	}
	_db.rollback();
	return false;
}

void Xeno::GetPlayers(Xeno::Players & map) const
{
	QSqlQuery query(_db);
	query.prepare(
				"SELECT `_playerID`, `name` FROM `Players`"
				);
	query.exec();
	qDebug()<<"GetPlayers :"<<query.lastError();
	Player player;
	while (query.next()) {
		player.playerID = query.value(0).toUInt();
		player.name = query.value(1).toString();
		map.insert(player.playerID, player);
	}
}

void Xeno::GetOpponents(uint32_t id, Xeno::Players & map) const
{
	QSqlQuery query(_db);
	query.prepare(
				"SELECT `_playerID`, `name` FROM "
				"(SELECT `playerIDw` AS _id FROM `Parties` WHERE `playerIDb` = :id "
				"UNION "
				"SELECT `playerIDb` AS _id FROM `Parties` WHERE `playerIDw` = :id "
				"GROUP BY `_id`) "
				"INNER JOIN `Players` ON `Players`.`_playerID` = `_id`;"
				);
	query.bindValue(":id", id);
	query.exec();
	qDebug()<<"GetOpponents :"<<query.lastError();
	Player player;
	while (query.next()) {
		player.playerID = query.value(0).toUInt();
		player.name = query.value(1).toString();
		map.insert(player.playerID, player);
	}
}

void Xeno::GetParties(uint32_t id1, uint32_t id2, Xeno::Parties & map) const
{
	QSqlQuery query(_db);
	if (id2 != 0) {
		query.prepare(
					"SELECT `_partyID`, `playerIDb`, `playerIDw`, `dateStart`, `dateFinish` "
					"FROM `Parties` "
					"WHERE "
						"(`playerIDb` = :id1 OR `playerIDw` = :id1) "
						"AND "
						"(`playerIDb` = :id2 OR `playerIDw` = :id2)"

					);
		query.bindValue(":id1", id1);
		query.bindValue(":id2", id2);
	}
	else {
		query.prepare(
					"SELECT `_partyID`, `playerIDb`, `playerIDw`, "
							"`dateStart`, `dateFinish` "
					"FROM `Parties` "
					"WHERE "
						"(`playerIDb` = :id1 OR `playerIDw` = :id1)"
					);
		query.bindValue(":id1", id1);
	}
	query.exec();
	qDebug()<<"GetParties :"<<query.lastError();
	Party party;
	while (query.next()) {
		party._partyID		= query.value(0).toUInt();
		party.playerIDb		= query.value(1).toUInt();
		party.playerIDw		= query.value(2).toUInt();
		party.dateStart		= QDateTime::fromSecsSinceEpoch(query.value(3).toUInt());
		party.dateFinish	= QDateTime::fromSecsSinceEpoch(query.value(4).toUInt());
		map.insert(party._partyID, party);
	}
}

void Xeno::GetMoves(uint32_t idParty, Xeno::Moves & list) const
{
	QSqlQuery query(_db);
	query.prepare(
				"SELECT "
					"`partyID`, `ordinal`, `snap`, `notation`, `player`, "
					"`strike`, `delay`, `comment` "
				"FROM `Moves` "
				"WHERE `partyID` = :ID "
				"ORDER BY `ordinal`;"
				);
	query.bindValue(":ID", idParty);
	if (query.exec()) {
		Xeno::Move move;
		while (query.next()) {
			QByteArray arr;
			arr					= query.value(2).toByteArray();
			QDataStream stream(&arr, QIODevice::ReadOnly);
			stream >> move.snap;
			move.partyID		= query.value(0).toUInt();
			move.ordinal		= query.value(1).toInt();
			move.notation		= query.value(3).toString();
			move.currentPlayer	= query.value(4).toBool() ? white : black;
			move.strike			= query.value(5).toBool();
//			move.delay			= query.value(6).toDateTime();
			move.comment		= query.value(7).toString();
			list.append(move);
		}
	}
	qDebug()<<"GetMoves: "<<query.lastError();
}

