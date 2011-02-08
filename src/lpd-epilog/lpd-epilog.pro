TARGET = lpd-epilog
macx {
  LIBS += -framework CoreFoundation
}
include(common.pri)

LIBS += -lcups

SOURCES += lpd.c network.c snmp-supplies.c runloop.c

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
