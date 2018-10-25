#pragma once

#include <QWidget>

#include "_board_global.h"
#include "_comm_types.h"
#include "xeno.h"
#include "solver.h"

class QGridLayout;

class _BOARDSHARED_EXPORT Board : public QWidget
{
		Q_OBJECT
public:
	struct	Style {
		int		cellWidth = 75;
		int		width;
		int		height;
		QPixmap	imgB;
		QPixmap	imgBS;
		QPixmap	imgW;
		QPixmap	imgWS;
	};

	explicit					Board(Solver * solver, QWidget* parent = nullptr);
								~Board();
					void		Flush();
					void		Fill();
					void		SetMove(Xeno::Move move);
					void		Reset(int row = 8, int col = 8);
					void		Pause(bool mode);
					Xeno::Move	move();
					bool		pause();

private:
					class	Label;
					class	Cell;

	static  const   QString                 _letters;
					Solver *				_solver;
					Solver::Sieve::Moves *	_moves;
					Style					_style;
					QGridLayout *           _layout;
					Xeno::Move				_initMove;
					Xeno::Move				_move;
					QPoint                  _startPoint;
					bool                    _pause			= false;
					uint32_t                _picked;
					uint32_t                _dropped;

					void		_FlushCells();
					bool		_PickUpCheck(uint32_t position);
					void		_DropCheck(uint32_t position);
					QString		_Pos2Note(uint32_t position);

signals:
					void		Restyle();
					void		ShowSnap();
					void		SetDrops(uint32_t const &drops);
					void		SendMove(Xeno::Move _move);
					void        SendGO();
private slots:
//					void		CatchDrop();
};
