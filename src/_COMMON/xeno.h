#pragma once

#include <QString>
#include <QDateTime>
#include <QtSql/QSqlDatabase>
#include <QMap>
#include <QList>

#include "_comm_types.h"

class Xeno
{
public:
	struct Move
	{
		struct Snap {
				uint32_t B = 0b0;
				uint32_t W = 0b0;
				uint32_t S = 0b0;
		} snap;
		uint32_t	partyID;
		Color		currentPlayer	= white;
		QString		notation		= " ";
		QString		comment;
		int			ordinal			= 0;
		int			delay;
		bool		strike			= false;
	};
	struct Player {
		uint32_t    playerID		= 0;
		QString     name;
	};
	struct Party {
		uint32_t    _partyID		= 0;
		uint32_t    playerIDw;
		uint32_t    playerIDb;
		QDateTime   dateStart;
		QDateTime   dateFinish;
	};

	typedef			QMap<uint32_t, Party>	Parties;
	typedef			QMap<uint32_t, Player>	Players;
	typedef			QList<Move>				Moves;

									Xeno();
									~Xeno();
					bool			OpenDB(QString path);
					uint32_t		AddPlayer(const Player & player);
					uint32_t		AddParty(const Party & party);
					bool			AddMove(const Party & party, const Move & move);
					bool			UpdateParty(const Party & party);
					void			GetPlayers(Players & map) const;
					void			GetOpponents(uint32_t id, Players & map) const;
					void			GetParties(uint32_t id1, uint32_t id2, Parties & map) const;
					void			GetMoves(uint32_t idParty, Moves & list) const;

private:
					QSqlDatabase    _db;
};
