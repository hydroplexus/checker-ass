#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListView>
#include <QComboBox>
#include <QPushButton>

#include "participant.h"

Participant::Participant(QWidget * parent) : QWidget(parent)
{
	_add		= new QPushButton(this);
	_selector	= new QComboBox(this);
	_view		= new QListView(_selector);
	_selector->setView(_view);

	_name.hide();
	_name.setAlignment(Qt::AlignmentFlag::AlignCenter);
	_name.setFixedHeight(44);
	_name.setFixedWidth(250);
	_name.setFont(QFont("Arial", 14, QFont::Bold));

	_add->setFixedHeight(_selector->height()-10);
	_add->setFixedWidth(_add->height());
	_add->setFlat(true);
	_add->setText("+");

	QHBoxLayout * lytH = new QHBoxLayout(this);
	QVBoxLayout * lytV = new QVBoxLayout;

	lytH->setSpacing(0);
	lytV->addWidget(& _name);
	lytV->addWidget(_selector);
	lytH->addLayout(lytV);
	lytH->addWidget(_add);

// сигналы, слоты
	{
		QObject::connect(this, &QObject::objectNameChanged,
						this, [ this ](const QString & name) {
							if (name == "B") {
								_name.setStyleSheet(
									"QLabel {"
												"background-color: black; "
												"color: white; "
									"}"
								);
							}
							if (name == "W") {
								_name.setStyleSheet(
									"QLabel {"
												"background-color: white; "
												"color: black; "
									"}"
								);
							}
						},
						Qt::DirectConnection);

		QObject::connect(_selector,
					static_cast<void(QComboBox::*)(int)>
								(&QComboBox::currentIndexChanged),
					this, [this](int idx) {
						if (idx == -1) {
							_name.setText("");
						}
						else {
							_name.setText(_selector->currentText());
							emit ParticipantChanged();
						}
					},
					Qt::DirectConnection);
		QObject::connect(_add, &QPushButton::clicked,
					this, &Participant::ParticipantAddPressed,
					Qt::DirectConnection);
	}
}

void Participant::Fill(const Xeno::Players & list)
{
	_selector->blockSignals(true);
	_selector->clear();
	_selector->insertItem(0, "*", 0);
	for (auto const & player : list) {
		_selector->addItem(player.name, player.playerID);
	}
	_selector->model()->sort(Qt::AscendingOrder);
	_selector->blockSignals(false);
	Reset();
}

void Participant::Exclude(const Xeno::Players & list, bool any)
{
	for (int row = 0; row < _selector->count(); row++) {
		bool show = list.contains(_selector->itemData(row).toUInt()) ;
		_view->setRowHidden(row, !show);
	}
	if (any) ShowAny();
}

void Participant::SetAny()
{
	_selector->setCurrentIndex(0);
}

void Participant::Reset()
{
	_selector->setCurrentIndex(-1);
}

void Participant::Clear()
{
	_selector->clear();
	_name.setText("");
}

uint32_t Participant::id()
{
	return _selector->currentIndex() == -1 ? 0 : _selector->currentData().toUInt();
}

QString Participant::name()
{
	return _selector->currentText();
}

void Participant::Add(const Xeno::Player & player, bool select)
{
	_selector->addItem(player.name, player.playerID);
	if (select) _selector->setCurrentIndex(_selector->count()-1);
	_selector->model()->sort(Qt::AscendingOrder);
}

void Participant::Set(uint32_t id)
{
	_selector->setCurrentIndex(_selector->findData(id));
}

void Participant::ShowAll()
{
	for (int row = 0; row < _selector->count(); row++) {
		_view->setRowHidden(row, false);
	}
}

void Participant::HideAll()
{
	for (int row = 0; row < _selector->count(); row++) {
		_view->setRowHidden(row, true);
	}
}

void Participant::ShowAny()
{
	_view->setRowHidden(0, false);
}

void Participant::HideAny()
{
	_view->setRowHidden(0, true);
}

void Participant::ChangeMode(Mode mode)
{
	switch (mode) {
		case viewPre:
			{
				Reset();
				_add->hide();
				_name.hide();
				_selector->show();
				break;
			}
		case gamePre:
			{
				HideAny();
				Reset();
				_add->show();
				_selector->show();
				_name.hide();
				break;
			}
		case gameStart:
			{
				_add->hide();
				_selector->hide();
				_name.show();
				break;
			}
	}
}
