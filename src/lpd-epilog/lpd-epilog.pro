TARGET = lpd-epilog
macx {
  LIBS += -framework CoreFoundation
}
include(common.pri)

LIBS += -lcups
QMAKE_CXXFLAGS += -I/usr/include/cups/
SOURCES += lpd.c network.c runloop.c

HEADERS  += \
            config.hpp \
            http-private.hpp \
            backend-private.hpp \
            debug.hpp \
            i18n.hpp \
            ipp-private.hpp \
            md5.hpp \
            ppd-private.hpp \
            cups-string.h
