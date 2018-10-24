#pragma once

#include <QWidget>

#include "_comm_types.h"
#include "xeno.h"

class DialogAdd;
class QLabel;
class QPushButton;
class QListWidget;
class QTableWidget;

class Board;
class Participant;

class UI : public QWidget
{
	Q_OBJECT

public:
	explicit					UI(QWidget * parent = nullptr);
								~UI();

private:
				Mode			_mode;
				Xeno            _xeno;
				Xeno::Party     _party;
				Xeno::Moves		_moves;
				Xeno::Players *	_players;
				Xeno::Parties * _parties;
				Board *			_board;
				QListWidget *   _lister;
				QTableWidget *	_tabler;
				int				_rowPrevious		= -1;
				Participant *   _playerB;
				Participant *   _playerW;
				QLabel *        _score;
				QPushButton *	_btnCtrlFinish		= nullptr;
				QPushButton	*	_btnCtrlStart;
				QPushButton	*	_btnCtrlMode;
				QPushButton	*	_btnCtrlPause;
				QPushButton	*	_btnCtrlViewFW;
				QPushButton	*	_btnCtrlViewBW;
				DialogAdd *		_dialogAddPlayer;

				void			ChangeMode(Mode mode);
				void			CreateParty();
				void			FillParties(const Xeno::Parties & list);
				void			Add2Lister(const Xeno::Move & move);
				void			SetCount();
				int				countCheckers(uint32_t snap);
				QString			looser();
				QString			winner();

protected:
				void			closeEvent(QCloseEvent * event);

signals:
				void			ModeChanged(Mode mode);

private slots:
				void			ParticipantChanged();
				void			CreateParticipant();
				void			CatchMove(Xeno::Move move);
				void			CatchGO();
				void			CatchParty(int row, int col);
				void			RestoreParty();
};
