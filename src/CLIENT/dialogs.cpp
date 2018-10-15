#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

#include "dialogs.h"

DialogAdd::DialogAdd(QWidget * parent) : QDialog(parent)
{
	QLabel * greetLabel = new QLabel(this);
	QLabel * tipLabel = new QLabel(this);
	QLineEdit * nameEdit = new QLineEdit(this);
	QPushButton * ok = new QPushButton(this);
	QPushButton * no = new QPushButton(this);

	greetLabel->setText("Мы все рады приветствовать нового участника!\n"
						"Не могли бы Вы представиться?");
	tipLabel->setText("Имя игрока не может быть пустым, "
					   "или длиннее 21 знака");
	nameEdit->setMaxLength(21);
	ok->setText("Подтвердить");
	no->setText("Отменить");

	QVBoxLayout * lytV = new QVBoxLayout(this);
	QHBoxLayout * lytH = new QHBoxLayout();

	lytV->addWidget(greetLabel);
	lytV->addWidget(nameEdit);
	lytV->addLayout(lytH);
	lytH->addWidget(ok);
	lytH->addWidget(no);
	lytV->addWidget(tipLabel);

	QObject::connect(no, &QPushButton::clicked,
			this, [ this, nameEdit ]() {
					nameEdit->clear();
					QDialog::reject();
			},
			Qt::DirectConnection);
	QObject::connect(ok, &QPushButton::clicked,
			this, [ this, nameEdit ]() {
				if(nameEdit->text() != "") {
					_player->name = nameEdit->text().trimmed();
					nameEdit->clear();
					QDialog::accept();
				}
			},
			Qt::DirectConnection);
	setWindowTitle("Добавление нового игрока");
}

int DialogAdd::exec(Xeno::Player & player)
{
	_player = &player;
	return QDialog::exec();
}
