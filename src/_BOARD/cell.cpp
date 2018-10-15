#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

#include "cell.h"
#include "solver.h"

Board::Cell::Cell(Board* parent, uint32_t position)
    : QLabel (parent)
{
    board = parent;
    this->position = position;
    setFrameShape(QFrame::NoFrame);
    setAlignment(Qt::AlignCenter);
    QObject::connect(parent, &Board::Restyle,
                     this, &Board::Cell::Restyle,
                     Qt::DirectConnection);
    if (position != 0) {
            QObject::connect(parent, &Board::ShowSnap,
                             this, &Board::Cell::ShowCheck,
                             Qt::DirectConnection);
            QObject::connect(parent, &Board::SetDrops,
                             this, &Board::Cell::SetDrop,
                             Qt::DirectConnection);
    }
}

Board::Cell::~Cell()
{

}

void Board::Cell::Restyle()
//	настройка внешнего вида клетки
{
    int width = board->_style.cellWidth;
    setFixedSize(width, width);
    if (position == 0) {
            setStyleSheet("background-color : white;");
        }
    else
        {
            setStyleSheet("background-color : black");
        }
}

void Board::Cell::HideCheck()
//	скрыть фигуру в текущей клетке
{
    setPixmap(QPixmap());
}

void Board::Cell::ShowCheck()
//	отобразить фигуру
{
    Xeno::Move::Snap snap = board->_move.snap;
    if (position & (snap.B | snap.W)) {
        QPixmap* img;
        if (position & snap.B) {
                img = (position & snap.S) ? &board->_style.imgBS : &board->_style.imgB;
            }
        else {
                img = (position & snap.S) ? &board->_style.imgWS : &board->_style.imgW;
            }
        setPixmap(*img);
    }
    else
        setPixmap(QPixmap());
}

void Board::Cell::SetDrop(const uint32_t & drops)
{
	setAcceptDrops(position & drops ? true : false);
}

void Board::Cell::mousePressEvent(QMouseEvent *event)
{
    board->_startPoint = event->pos();
//	return QLabel::mousePressEvent(event);
}

void Board::Cell::mouseMoveEvent(QMouseEvent *event)
{
if (
	(!board->_pause) &&
    (event->buttons()&Qt::LeftButton) &&
    QApplication::startDragDistance()<=(event->pos()-board->_startPoint).manhattanLength())
    {
        if (board->_PickUpCheck(position)) {
            QDrag * checker = new QDrag(this);
            checker->setPixmap(*pixmap());
            QMimeData * mime = new QMimeData;
            checker->setMimeData(mime);
            HideCheck();
            Qt::DropAction result = checker->exec(Qt::MoveAction);
            if (result != Qt::MoveAction) {ShowCheck();}
        }
    }
}

void Board::Cell::dragEnterEvent(QDragEnterEvent *event)
{
        event->acceptProposedAction();
}

void Board::Cell::dropEvent(QDropEvent *event)
{
    board->_DropCheck(position);
    event->proposedAction();
}
