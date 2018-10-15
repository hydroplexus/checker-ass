#pragma once

#include <QLabel>

#include "board.h"

class Board::Label : public QLabel
{
        Q_OBJECT
public:
            Board*  board;
            bool    isLetter;
explicit            Label(Board* parent, bool isLetter, int symbol);
                    ~Label();

public slots:
            void    Restyle();
};
