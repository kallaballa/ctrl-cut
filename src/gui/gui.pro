TARGET = ctrl-cut
CONFIG += boost cups gsapi
include(common.pri)
CONFIG += qt
CONFIG += app_bundle
QT += network

MOC_DIR = objects
UI_DIR = objects
RCC_DIR = objects

macx {
  LIBS += -framework CoreFoundation
}

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API

INCLUDEPATH += ../cups-filter ../lpd-epilog

FORMS   += MainWindow.ui

RESOURCES += ctrl-cut.qrc

HEADERS += MainWindow.h \
           NavigationView.h \          
           LpdClient.h \
           ../cups-filter/vector/Geometry.h \
           ../cups-filter/vector/CutModel.h \
           ../cups-filter/util/LaserConfig.h \
           ../cups-filter/util/Eps.h \
           ../cups-filter/FileParser.h \
           ../cups-filter/tri_logger/tri_logger.hpp \
           ../cups-filter/tri_logger/nullstream.hpp \
           ../lpd-epilog/backend-private.h \
           ../lpd-epilog/config.h \
           ../lpd-epilog/cups-string.h \
           ../lpd-epilog/debug.h \
           ../lpd-epilog/http-private.h \
           ../lpd-epilog/i18n.h \
           ../lpd-epilog/ipp-private.h \
           ../lpd-epilog/md5.h \
           ../lpd-epilog/ppd-private.h \
           ../lpd-epilog/snmp-private.h

SOURCES += gui.cpp \
           MainWindow.cpp \
           NavigationView.cpp \
           LpdClient.cpp \
           ../cups-filter/vector/CutModel.cpp \
           ../cups-filter/util/LaserConfig.cpp \
           ../cups-filter/util/Eps.cpp \
           ../cups-filter/PostscriptParser.cpp \
           ../cups-filter/tri_logger/tri_logger.cpp \
           ../lpd-epilog/network.c \
           ../lpd-epilog/runloop.c \
           lpd-client.c

