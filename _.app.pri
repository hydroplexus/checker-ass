DESTDIR = $${BIN_PATH}
linux-g++: QMAKE_LFLAGS += -Wl,--rpath=$${LIBS_PATH}
