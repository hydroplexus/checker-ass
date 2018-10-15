#pragma once

#include <QtCore/qglobal.h>

#if defined(_BOARD_LIBRARY)
#  define _BOARDSHARED_EXPORT Q_DECL_EXPORT
#else
#  define _BOARDSHARED_EXPORT Q_DECL_IMPORT
#endif

