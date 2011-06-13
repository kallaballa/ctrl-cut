TARGET = ctrl-cut
CONFIG += boost cups gsapi
include(common.pri)
CONFIG += qt
CONFIG += app_bundle

MOC_DIR = objects
UI_DIR = objects
RCC_DIR = objects

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API

INCLUDEPATH += ../cups-filter

FORMS   += MainWindow.ui

RESOURCES += ctrl-cut.qrc

HEADERS += MainWindow.h \
           NavigationView.h \
           ../cups-filter/vector/Cut.h \
           ../cups-filter/vector/Vertex.h \
           ../cups-filter/vector/Edge.h \
           ../cups-filter/vector/Polyline.h \
           ../cups-filter/vector/Mesh.h \
           ../cups-filter/util/LaserConfig.h \
           ../cups-filter/util/Eps.h \
           ../cups-filter/FileParser.h \
           ../cups-filter/tri_logger/tri_logger.hpp \
           ../cups-filter/tri_logger/nullstream.hpp

SOURCES += gui.cpp \
           MainWindow.cpp \
           NavigationView.cpp \
           ../cups-filter/vector/Cut.cpp \
           ../cups-filter/vector/Vertex.cpp \
           ../cups-filter/vector/Edge.cpp \
           ../cups-filter/vector/Polyline.cpp \
           ../cups-filter/vector/Mesh.cpp \
           ../cups-filter/util/LaserConfig.cpp \
           ../cups-filter/util/Eps.cpp \
           ../cups-filter/PostscriptParser.cpp \
           ../cups-filter/tri_logger/tri_logger.cpp
