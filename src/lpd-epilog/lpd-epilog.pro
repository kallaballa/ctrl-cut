TARGET = lpd-epilog
macx {
  LIBS += -framework CoreFoundation
}
include(common.pri)

LIBS += -lcups

SOURCES += lpd.c

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
