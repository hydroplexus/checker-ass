#-------------------------------------------------
#
# Project created by QtCreator 2018-03-30T17:02:37
#
#-------------------------------------------------

QT      += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CLIENT
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
	main.cpp \
    participant.cpp \
    ui.cpp \
    dialogs.cpp

HEADERS += \
    participant.h \
    ui.h \
    dialogs.h

include(../../_.common.pri)
include(../../_.app.pri)


#link Board::
win32:CONFIG(release, debug|release): LIBS += -l_BOARD
else:win32:CONFIG(debug, debug|release): LIBS += -l_BOARD
else:unix:!macx: LIBS += -l_BOARD

#link static _COMMON
win32:CONFIG(release, debug|release): LIBS += -l_COMMON
else:win32:CONFIG(debug, debug|release): LIBS += -l_COMMON
else:unix:!macx: LIBS += -l_COMMON

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $${LIBS_PATH}/lib_COMMON.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $${LIBS_PATH}/lib_COMMON.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $${LIBS_PATH}/_COMMON.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $${LIBS_PATH}/_COMMON.lib
else:unix:!macx: PRE_TARGETDEPS += $${LIBS_PATH}/lib_COMMON.a

FORMS +=

DISTFILES += \
    ../../_.doc/_.chk-ass-UI.qmodel


