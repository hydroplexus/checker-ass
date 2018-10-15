#pragma once

#include <QDialog>

#include "xeno.h"

class DialogAdd : public QDialog
{
public:
						DialogAdd(QWidget * parent = nullptr);
		int				exec(Xeno::Player & player);

private:
		Xeno::Player *	_player;
};
