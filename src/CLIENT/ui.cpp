#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QProxyStyle>
#include <QMessageBox>

#include "ui.h"
#include "participant.h"
#include "dialogs.h"
#include "board.h"
#include "solver.h"

class StyleProxyNonOutlined : public QProxyStyle {
//	прокси для подавления отрисовки бордюра при выделении клеток
public:
  void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
					 QPainter * painter, const QWidget * widget = nullptr) const
  {
	if (element == PE_FrameFocusRect)
	{
	 return;
	}
	QProxyStyle::drawPrimitive(element, option, painter, widget);
  }
};

UI::UI(QWidget * parent) : QWidget(parent)
{
//	Q_INIT_RESOURCE(_board);
//	TODO	перенести сюда все необязательные элементы интерфейса из заголовка
//  создаём элементы интерфейса
	{
		_dialogAddPlayer	= new DialogAdd(this);
		_btnCtrlMode		= new QPushButton(this);
		_btnCtrlStart		= new QPushButton(this);
		_btnCtrlPause		= new QPushButton(this);
		_btnCtrlViewFW		= new QPushButton(this);
		_btnCtrlViewBW		= new QPushButton(this);
		_playerB			= new Participant(this);
		_playerW			= new Participant(this);
		_score				= new QLabel(this);
		_lister				= new QListWidget(this);
		_tabler				= new QTableWidget(this);
		_board				= new Board(new Solver, this);
		_board->Reset();
	}

//	ставим растяжки, расставляем элементы
	{
		QHBoxLayout * lytUI			= new QHBoxLayout(this);
		QVBoxLayout * lytLeft		= new QVBoxLayout;
		QHBoxLayout * lytViewCtrl	= new QHBoxLayout;
		QVBoxLayout * lytMiddle		= new QVBoxLayout;
		QHBoxLayout * lytMiddleTop	= new QHBoxLayout;
		QVBoxLayout * lytRight		= new QVBoxLayout;

		lytUI->setSizeConstraint(QLayout::SetFixedSize);
		lytUI->addLayout(lytLeft);
		lytUI->addLayout(lytMiddle);
		lytUI->addLayout(lytRight);
		lytMiddle->addLayout(lytMiddleTop);
		lytViewCtrl->setSpacing(0);

		lytLeft->addWidget(_btnCtrlMode);
		lytLeft->addWidget(_btnCtrlPause);
		lytLeft->addLayout(lytViewCtrl);
		lytViewCtrl->addWidget(_btnCtrlViewBW);
		lytViewCtrl->addWidget(_btnCtrlViewFW);
		lytLeft->addWidget(_lister);
		lytMiddle->addWidget(_board);
		lytMiddleTop->addWidget(_playerB);
		lytMiddleTop->addWidget(_btnCtrlStart);
		lytMiddleTop->addWidget(_score);
		lytMiddleTop->addWidget(_playerW);
		lytRight->addWidget(_tabler);
	}

//	оформляем элементы
	{
		_playerB->setObjectName("B");
		_playerW->setObjectName("W");
		_score->setAlignment(Qt::AlignCenter);
		_score->setFont(QFont("Courier", 14, QFont::Bold));
		_btnCtrlStart->setText("НАЧАТЬ ИГРУ");
		_btnCtrlViewBW->setText("<<");
		_btnCtrlViewFW->setText(">>");
		_btnCtrlViewBW->setFixedWidth(44);
		_btnCtrlViewFW->setFixedWidth(44);
		_lister->setFixedWidth(88);

		_tabler->setFixedWidth(377);
		_tabler->setColumnCount(4);
		_tabler->setShowGrid(false);

//FIXME	после сортировки, при попытке перестройки таблицы
//		segfault в setData(Qt::UserRole, player._playerID)
///		_tabler->setSortingEnabled(true);

		_tabler->verticalHeader()->hide();
		_tabler->setStyle(new StyleProxyNonOutlined);
		_tabler->setSelectionBehavior(QAbstractItemView::SelectRows);
		_tabler->setEditTriggers(QAbstractItemView::NoEditTriggers);
		_tabler->setSelectionMode(QAbstractItemView::SingleSelection);
		_tabler->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		_tabler->setHorizontalHeaderLabels(QStringList()
										   <<"Начата"
										   <<"Чёрные"
										   <<"Белые"
										   <<"Окончена");
	}

//	связываем сигналы
		{
			QObject::connect(_playerB, &Participant::ParticipantChanged,
							this, &UI::ParticipantChanged,
							Qt::DirectConnection);
			QObject::connect(_playerW, &Participant::ParticipantChanged,
							this, &UI::ParticipantChanged,
							Qt::DirectConnection);

			QObject::connect(_playerB, &Participant::ParticipantAddPressed,
							 this, &UI::CreateParticipant,
							 Qt::DirectConnection);
			QObject::connect(_playerW, &Participant::ParticipantAddPressed,
							 this, &UI::CreateParticipant,
							 Qt::DirectConnection);

			QObject::connect(this, &UI::ModeChanged,
							_playerB, &Participant::ChangeMode,
							Qt::DirectConnection);
			QObject::connect(this, &UI::ModeChanged,
							_playerW, &Participant::ChangeMode,
							Qt::DirectConnection);

			QObject::connect(_board, &Board::SendMove,
							this, &UI::CatchMove,
							Qt::DirectConnection);
			QObject::connect(_board, &Board::SendGO,
							this, &UI::CatchGO,
							Qt::DirectConnection);
			QObject::connect(_tabler, &QTableWidget::cellDoubleClicked,
							this, &UI::CatchParty,
							Qt::DirectConnection);
		}

//	связываем нажания на кнопки
//	TODO	утащить одноразовое в лямбды
	{
		QObject::connect(_btnCtrlMode, &QPushButton::clicked,
						this,	[ this ]() {
							ChangeMode(_mode == viewPre ? gamePre : viewPre);
						},
						Qt::DirectConnection);
		QObject::connect(_btnCtrlStart, &QPushButton::clicked,
						this,	[ this ]() {
							CreateParty();
							ChangeMode(gameStart);
						},
						Qt::DirectConnection);
		QObject::connect(_btnCtrlPause, &QPushButton::clicked,
						this,	[ this ]() {
							ChangeMode(_board->pause() ? gamePlay : gamePause);
						},
						Qt::DirectConnection);

		QObject::connect(_btnCtrlViewFW, &QPushButton::clicked,
						this, [ this ]() {
							int row = _lister->currentRow();
							if (row < _lister->count() - 1) {_lister->setCurrentRow(row + 1);}
						},
						Qt::DirectConnection);
		QObject::connect(_btnCtrlViewBW, &QPushButton::clicked,
						this, [ this ]() {
							int row = _lister->currentRow();
							if (row != 0) {_lister->setCurrentRow(row - 1);}
						},
						Qt::DirectConnection);

		QObject::connect(_lister, &QListWidget::currentRowChanged,
						this, [ this ](int row) {
							if (row >= 0 && _mode == viewPre) {_board->SetMove(_moves.at(row));}
						},
						Qt::DirectConnection);
	}

	_xeno.OpenDB("../../src/_COMMON/_.res/xeno.sqlite3");

	_players = new Xeno::Players;
	_parties = new Xeno::Parties;
	_xeno.GetPlayers(*_players);
	_playerB->Fill(*_players);
	_playerW->Fill(*_players);

	ChangeMode(gamePre);
}

void UI::ParticipantChanged()
{
	uint32_t B = _playerB->id();
	uint32_t W = _playerW->id();
	bool isBlack = sender()->objectName() == "B";
	qDebug()<<"ParticipantChanged: Black "<<isBlack<<", B: "<<B<<", W: "<<W;
	switch (_mode) {
		case gamePre : {
				if (B == W) {
					(isBlack ? _playerW : _playerB)->Reset();
					_score->show();
					_btnCtrlStart->hide();
					break;
				}
				if (B != 0 && W != 0) {
					_score->hide();
					_btnCtrlStart->show();
				}
				break;
			}
		case viewPre : {
				if (isBlack) {
					Xeno::Players map;
					_xeno.GetOpponents(B, map);
					_playerW->Exclude(map, true);
					_parties->clear();
					_xeno.GetParties(B, 0, *_parties);
					int row = _parties->count();
					_tabler->setRowCount(0);
					_tabler->setRowCount(row);
					for (auto const & party : *_parties) {
						--row;
						_tabler->setItem(row, 0, new QTableWidgetItem(party.dateStart.date().toString("dd.MM.yyyy")));
						_tabler->setItem(row, 1, new QTableWidgetItem(_players->value(party.playerIDb).name));
						_tabler->setItem(row, 2, new QTableWidgetItem(_players->value(party.playerIDw).name));
						if (party.dateFinish.toSecsSinceEpoch() != 0) {
							_tabler->setItem(row, 3, new QTableWidgetItem(party.dateFinish.date().toString("dd.MM.yyyy")));
						}
						_tabler->item(row, 0)->setData(Qt::UserRole, party._partyID);
						_tabler->item(row, 0)->setData(Qt::UserRole + 1,
													   party.playerIDb == B ?
													   party.playerIDw : party.playerIDb);
					}
					_rowPrevious = -1;
					_playerW->SetAny();
				}
				else {
					for (int row = _tabler->rowCount()-1; row >=0; row--) {
						if (_tabler->item(row, 0)->data(Qt::UserRole + 1).toUInt() == W
								|| W == 0) {
							_tabler->showRow(row);
						}
						else {
							_tabler->hideRow(row);
						}
					}
				}
				break;
		}
	}
}

void UI::CreateParty()
{
	_party.playerIDb = _playerB->id();
	_party.playerIDw = _playerW->id();
	_party.dateStart = QDateTime::currentDateTime();
	_party._partyID  = _xeno.AddParty(_party);
	_moves.clear();
	_moves.append(_board->move());
	_xeno.AddMove(_party, _board->move());
}

void UI::RestoreParty()
{
	_playerB->Set(_party.playerIDb);
	_playerW->Set(_party.playerIDw);
	_lister->setCurrentRow(_lister->count()-1);
	ChangeMode(gameRestore);
}

void UI::CreateParticipant()
{
	Xeno::Player player;
	_dialogAddPlayer->exec(player);
	if (_dialogAddPlayer->result() == QDialog::Accepted) {
		player.playerID = _xeno.AddPlayer(player);
		if (player.playerID == 0) {
			QMessageBox::warning(this,
					"НЕУДАЧНО !",
					"Возможно, игрок с таким именем уже присутствует",
					QMessageBox::Close);
			return;
		}
		_players->insert(player.playerID, player);
		bool who = sender()->objectName() == "B";
		_playerB->Add(player, who);
		_playerW->Add(player, !who);
	}
}

void UI::Add2Lister(const Xeno::Move & move)
{
	if (move.ordinal == 0) {
			_lister->addItem("начальная позиция");
			_lister->item(_lister->count()-1)->setHidden(true);
	}
	else {
		_lister->addItem(QString::number(move.ordinal) + ") " + move.notation);
	}
	_lister->item(_lister->count()-1)->setData(Qt::UserRole, move.ordinal);
}

void UI::SetCount()
{
	QString count = QString::number((countCheckers(_moves.last().snap.B))) +
					" : " +
					QString::number((countCheckers(_moves.last().snap.W)));
	_score->setText(count);
}

int UI::countCheckers(uint32_t snap)
{
	int res = snap & 1;
	while (snap>>=1) {res += snap &1;};
	return res;
}

QString UI::looser()
{
	return _board->move().currentPlayer ? _playerW->name() : _playerB->name();
}

QString UI::winner()
{
	return _board->move().currentPlayer ? _playerB->name() : _playerW->name();
}

void UI::closeEvent(QCloseEvent * event)
{
	if ((_mode == gamePlay || _mode == gamePause) &&
			(QMessageBox::No ==
				QMessageBox::warning(this,
				"Закрытие приложения",
				"Вы ещё не одержали победу.\n"
				"Уверены, что хотите прервать партию?\n"
				"\n"
				"Хотя... её можно будет продолжить в другой раз.",
				QMessageBox::No | QMessageBox::Yes))) {
		event->ignore();
		return;
	}
	event->accept();
}

void UI::CatchMove(Xeno::Move move)
{
	_moves.append(move);
	_xeno.AddMove(_party, move);
	Add2Lister(move);
	_lister->setCurrentRow(_lister->count()-1);
	SetCount();
}

void UI::CatchGO()
{
	_party.dateFinish = QDateTime::currentDateTime();
	_xeno.UpdateParty(_party);
	QMessageBox::information(this,
		"Партия окончена",
		"Поздравляем, " + winner() + "!\n"
		"Вы только что одержали победу над противником.\n"
		"\n" + looser() +
		", вам советуем больше тренироваться и всё обязательно получится.",
		QMessageBox::Yes);
	ChangeMode(gamePre);

}

void UI::CatchParty(int row, int col)
{
	uint32_t party = _tabler->item(row, 0)->data(Qt::UserRole).toUInt();
	_party = (*_parties)[party];
	_moves.clear();
	_lister->clear();
	_xeno.GetMoves(_party._partyID, _moves);
	for (auto const & move : _moves) {
		Add2Lister(move);
	}
	ChangeMode(viewStart);
	_lister->setCurrentRow(0);
	_tabler->removeCellWidget(_rowPrevious, 3);
	if (_tabler->item(row, 3) == nullptr) {
		_btnCtrlFinish = new QPushButton("ДОИГРАТЬ");
		_tabler->setCellWidget(row, 3, _btnCtrlFinish);
		QObject::connect(_btnCtrlFinish, &QPushButton::clicked,
						this, &UI::RestoreParty,
						Qt::DirectConnection);
	}
	if (_rowPrevious != -1) {
		_tabler->item(_rowPrevious, 1)->setData(Qt::BackgroundRole, QVariant(QColor(Qt::white)) );
		_tabler->item(_rowPrevious, 1)->setData(Qt::ForegroundRole, QVariant(QColor(Qt::black)) );
	}
	_tabler->item(row, 1)->setData(Qt::BackgroundRole, QVariant(QColor(Qt::black)) );
	_tabler->item(row, 1)->setData(Qt::ForegroundRole, QVariant(QColor(Qt::white)) );

	_rowPrevious = row;
}

void UI::ChangeMode(Mode mode)
{
	switch (mode) {
		case viewPre:
			{
				if ((_mode == gamePlay || _mode == gamePause) &&
						(QMessageBox::No ==
							QMessageBox::warning(this,
							"Смена режима приложения",
							"Вы ещё не одержали победу.\n"
							"Уверены, что хотите прервать партию?\n"
							"\n"
							"Хотя... её можно будет продолжить в другой раз.",
							QMessageBox::No | QMessageBox::Yes))) {
					mode = _mode;
					break;
				}
				_btnCtrlMode->setText("АРХИВ");
				_btnCtrlViewBW->hide();
				_btnCtrlViewFW->hide();
				_btnCtrlPause->hide();
				_btnCtrlStart->hide();
				_score->setText("ВЫБЕРИТЕ ИГРОКОВ");
				_score->show();
				_lister->clear();
				_board->Flush();
				_board->Pause(true);
				_tabler->clearContents();
				_tabler->show();
				_playerW->HideAll();
				break;
			}
		case viewStart :
			{
				_btnCtrlViewBW->show();
				_btnCtrlViewFW->show();
				mode = viewPre;
				break;
			}
		case gamePre:
			{
				_btnCtrlMode->setText("ИГРА");
				_btnCtrlViewBW->hide();
				_btnCtrlViewFW->hide();
				_btnCtrlPause->hide();
				_btnCtrlStart->hide();
				_tabler->hide();
				_score->show();
				_score->setText("ВЫБЕРИТЕ ИГРОКОВ");
				_lister->clear();
				_board->Fill();
				_board->Pause(true);
				_playerW->ShowAll();
				break;
			}
		case gameRestore:
			{
				_btnCtrlMode->setText("ИГРА");
				_btnCtrlViewBW->hide();
				_btnCtrlViewFW->hide();
				_btnCtrlPause->hide();
				_btnCtrlStart->hide();
				_tabler->hide();
				_score->show();
				mode = gameStart;
			}
		[[fallthrough]];
		case gameStart:
			{
				_btnCtrlStart->hide();
				SetCount();
				_score->show();
				_btnCtrlPause->show();
				emit ModeChanged(mode);
				mode = gamePlay;
			}
		[[fallthrough]];
		case gamePlay:
			{
				_btnCtrlPause->setText("ПАУЗА");
				_board->Pause(false);
				break;
			}
		case gamePause:
			{
				_btnCtrlPause->setText("ДАЛЬШЕ");
				_board->Pause(true);
				break;
			}
	}
	if (_mode != mode) {
		_mode = mode;
		emit ModeChanged(_mode);
	}
}

UI::~UI()
{
//	Q_CLEANUP_RESOURCE(_board);
	delete _players;
	delete _parties;
}
