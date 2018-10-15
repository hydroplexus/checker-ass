#include "label.h"

Board::Label::Label(Board* parent, bool isLetter, int symbol)
	: QLabel (parent)
{
	board = parent;
	this->isLetter = isLetter;
	if (isLetter) {
			setText(Board::_letters.at(symbol));
		}
	else
		{
			setNum(symbol+1);
		}
	setAlignment(Qt::AlignCenter);
	setFrameShape(QFrame::NoFrame);
	QObject::connect(parent, &Board::Restyle,
					 this, &Label::Restyle,
					 Qt::DirectConnection);
}

Board::Label::~Label()
{

}

void Board::Label::Restyle()
// настройка внешнего вида клетки
// FIXME	выбор высоты символов
{
	int width, height;
	width = height = board->_style.cellWidth;
	setStyleSheet("font-weight:bold;");
	setStyleSheet(QString("font-size: %1px").arg(width/2.1));
	if (isLetter) {height/=3;} else {width/=3;}
	setFixedSize(width, height);
}
