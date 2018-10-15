H_FROM	= $$system_path($$_PRO_FILE_PWD_/*.h)
H_TO	= $$system_path($$EXPORT_PATH)

QMAKE_EXTRA_TARGETS	+= copyheaders
POST_TARGETDEPS		+= copyheaders
copyheaders.commands	= $(COPY) $$H_FROM $$H_TO

DESTDIR		    = $${LIBS_PATH}
win32: DLLDESTDIR   = $${BIN_PATH}

#VERSION = 0.0.1
#QMAKE_TARGET_COPYRIGHT = (c) npgt.ru
