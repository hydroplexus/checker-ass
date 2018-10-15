#pragma once

#include <QWidget>
#include <QLabel>

#include "_comm_types.h"
#include "xeno.h"

class QListView;
class QComboBox;
class QPushButton;

class Participant : public QWidget
{
	Q_OBJECT
public:
	explicit					Participant(QWidget * parent = nullptr);
				void			Fill(const Xeno::Players & list);
				void			Exclude(const Xeno::Players & list, bool any);
				void			Add(const Xeno::Player & player, bool select);
				void			Set(uint32_t id);
				void			ShowAll();
				void			HideAll();
				void			ShowAny();
				void			HideAny();
				void			SetAny();
				void            Reset();
				void			Clear();
				uint32_t		id();
				QString			name();

private:
				QListView *		_view;
				QComboBox *		_selector;
				QLabel			_name;
				QPushButton	*	_add;

signals:
				void			ParticipantChanged();
				void			ParticipantAddPressed();

public slots:
				void			ChangeMode(Mode mode);
};
