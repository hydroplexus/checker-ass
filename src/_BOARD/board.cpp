#include <QGridLayout>

#include "board.h"
#include "cell.h"
#include "label.h"

const QString Board::_letters = "abcdefgh";

Board::Board(Solver * solver, QWidget * parent) : QWidget (parent)
{
	_solver = solver;
	_moves = _solver->getMoves();
	_style.imgW.load(":/shape/imgW");
	_style.imgWS.load(":/shape/imgWS");
	_style.imgB.load(":/shape/imgB");
	_style.imgBS.load(":/shape/imgBS");
	_layout = new QGridLayout(this);
	_layout->setSpacing(0);
	_layout->setContentsMargins(0,0,0,0);
	_layout->setSizeConstraint(QLayout::SetFixedSize);
}

Board::~Board()
{
	delete _solver;
}

void Board::Flush()
{
	_move = _initMove;
	_move.snap.B = 0b0;
	_move.snap.W = 0b0;
	_move.snap.S = 0b0;
	_solver->SetMoves(_move.snap, _move.currentPlayer);
	emit ShowSnap();
}

void Board::Fill()
{
	_move = _initMove;
	_solver->SetMoves(_move.snap, _move.currentPlayer);
	emit ShowSnap();
}

void Board::SetMove(Xeno::Move move)
{
	_move = move;
	_solver->SetMoves(_move.snap, _move.currentPlayer);
	emit ShowSnap();
}

void Board::Reset(int row, int col)
{
	_initMove.snap.B = 0b11111111111100000000000000000000;
	_initMove.snap.W = 0b00000000000000000000111111111111;
	_initMove.snap.S = 0b0;

	if (row%2 || col%2 || row<8 || col<8 || row>10 || col>10) return;
	_style.width = col; col--;
	_style.height = row; row--;
	_FlushCells();
//расставляем координаты
	Label* coord;
	for (int cnt=0; cnt<_style.width; cnt++)	{
		coord = new Label(this, true, cnt); //верхняя буква
		_layout->addWidget(coord, 0, cnt+1);
		coord = new Label(this, true, cnt); //нижняя буква
		_layout->addWidget(coord, _style.height+1, cnt+1);
		if (cnt>=_style.height) continue;
		coord = new Label(this, false, cnt); //левая цифра
		_layout->addWidget(coord, _style.height-cnt, 0);
		coord = new Label(this, false, cnt); //правая цифра
		_layout->addWidget(coord, _style.height-cnt, _style.width+1);
	}
//расставляем клетки
	uint32_t num = 0b10000000000000000000000000000000;
	Cell* cell;
	for (int row_cnt=1; row_cnt<_style.height+1; row_cnt++)	{
		for (int col_cnt=1; col_cnt<_style.width+1; col_cnt++) {
			if (col_cnt%2 != row_cnt%2)
				{	//клетка черная
					cell = new Cell(this, num);
					num >>= 1;
					_layout->addWidget(cell, row_cnt, col_cnt);
				}
			else
				{	//клетка белая
					cell = new Cell(this);
					_layout->addWidget(cell, row_cnt, col_cnt);
				}
		}
	}
//задаём оформление и отображаем текущую позицию
	Flush();
	emit Restyle();
	emit ShowSnap();
	_layout->update();
}

void Board::Pause(bool mode)
{
	_pause = mode;
}

Xeno::Move Board::move()
{
	return _move;
}

bool Board::pause()
{
	return _pause;
}

void Board::_FlushCells()
{
	while (QLayoutItem * item = _layout->itemAt(0)) {
			_layout->removeItem(item);
//			layout_.removeWidget(item->widget());
			delete item->widget();
			delete item;
	}
}

bool Board::_PickUpCheck(uint32_t position)
{
	_picked = position;
	_move.notation = _Pos2Note(_picked);
	if (_moves->hunter & position) {
		emit SetDrops(_moves->drops[position].strike);
		return true;
	}
	if (_moves->unlocked & position) {
		emit SetDrops(_moves->drops[position].available);
		return true;
	}
	return false;
}

//TODO	перенести правила в Solver::
void Board::_DropCheck(uint32_t position)
{
//	переносим фигуру
	((_move.currentPlayer ? _move.snap.W : _move.snap.B) &= ~_picked) |= position;

//	переносим статус дамки
	if (_move.snap.S & _picked) {
			(_move.snap.S &= ~_picked) |= position;
	}
//	повышаем фигуру до дамки, если есть возможность
	_move.snap.S |= (position & _moves->drops[_picked].super);

//	обрабатываем взятие фигуры противника
	if (_moves->drops[_picked].strike & position) {
			uint32_t striked = 0;
			if (_moves->drops[_picked].strikes.contains(position)) {
					striked = _moves->drops[_picked].strikes[position];
			}
			else {
					for (auto const & key : _moves->drops[_picked].strikes.keys()) {
							if (key & position) {
								striked = _moves->drops[_picked].strikes[key];
								break;
							}
					}
			}
			(_move.currentPlayer ? _move.snap.B : _move.snap.W) &= ~striked;
			_move.snap.S &= ~striked;
			_move.strike = true;
	}
	else {
			_move.strike = false;
	}

//	проверяем, может ли фигура повторить взятие, если нет то передаём ход
	_solver->SetMoves(_move.snap, _move.currentPlayer);
	if ((_moves->hunter & position) && _move.strike) {
		_moves->unlocked = (_moves->hunter &= position);
	}
	else {
		_move.currentPlayer = _move.currentPlayer ? black : white;
		_solver->SetMoves(_move.snap, _move.currentPlayer);
		if (_moves->hunter) {
				_moves->unlocked = _moves->hunter;
			}
	}
	_move.notation +=  (_move.strike ? " : " : " - ") +
						_Pos2Note(position);
	_move.ordinal++;
	emit SendMove(_move);
	emit ShowSnap();
	if (!_moves->unlocked) emit SendGO();
}

QString Board::_Pos2Note(uint32_t position)
{
	int ord = 0;
	while (position>>=1) {ord++;};
	int num = ord/4+1;
	int lett = 7-(ord % 4)*2-(num % 2);
	return _letters.at(lett) + QString::number(num);
}
