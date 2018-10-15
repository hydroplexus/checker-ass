win32:	OS_SUFFIX = win32
linux:	OS_SUFFIX = linux
#linux-g++:  QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS += -std=c++17

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
    LIB_SUFFIX = d
}
CONFIG(release, debug|release) {
    BUILD_FLAG = release
}

ROOT_PATH   = $${PWD}
EXPORT_PATH = $${ROOT_PATH}/export/
IMPORT_PATH = $${ROOT_PATH}/import/
BIN_PATH    = $${ROOT_PATH}/bin/$${BUILD_FLAG}/
BUILD_PATH  = $${ROOT_PATH}/build/$${BUILD_FLAG}/$${TARGET}/

RCC_DIR	    = $${BUILD_PATH}rcc/
UI_DIR	    = $${BUILD_PATH}ui/
MOC_DIR	    = $${BUILD_PATH}moc/
OBJECTS_DIR = $${BUILD_PATH}obj/

INCLUDEPATH += $${EXPORT_PATH}
INCLUDEPATH += $${IMPORT_PATH}
DEPENDPATH  += $${EXPORT_PATH}

LIBS_PATH   = $${ROOT_PATH}/lib.$${OS_SUFFIX}/$${BUILD_FLAG}/
LIBS	    += -L$${LIBS_PATH}
