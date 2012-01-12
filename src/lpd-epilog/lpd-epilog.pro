TARGET = lpd-epilog
macx {
  LIBS += -framework CoreFoundation
}
include(common.pri)

LIBS += -lcups
QMAKE_CXXFLAGS += -I/usr/include/cups/
SOURCES += lpd.c network.c runloop.c

HEADERS  += \
            config.h \
            http-private.h \
            backend-private.h \
            debug.h \
            i18n.h \
            ipp-private.h \
            md5.h \
            ppd-private.h \
            cups-string.h
