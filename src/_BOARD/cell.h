#pragma once

#include <QLabel>

#include "board.h"
#include "solver.h"

class Board::Cell : public QLabel
{
        Q_OBJECT
public:
                Board*		board;
                uint32_t	position = 0;
    explicit                Cell(Board* parent, uint32_t position = 0);
                            ~Cell();
                void        HideCheck();

protected:
                void        mouseMoveEvent(QMouseEvent *event);
                void        mousePressEvent(QMouseEvent *event);
                void        dragEnterEvent(QDragEnterEvent *event);
                void        dropEvent(QDropEvent *event);

public slots:
                void        Restyle();
                void        ShowCheck();
				void        SetDrop(uint32_t const &drops);
};
