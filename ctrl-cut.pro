TARGET = ctrl-cut
#CONFIG += raster
CONFIG += gsapi
CONFIG -= qt
CONFIG += boost

isEmpty(VERSION) VERSION = $$system(date "+%Y.%m.%d")
DEFINES += CTRLCUT_VERSION=$$VERSION

macx {
  CONFIG -= app_bundle

  DEPLOYDIR = $$(MACOSX_DEPLOY_DIR)
  !isEmpty(DEPLOYDIR) {
    INCLUDEPATH += $$DEPLOYDIR/include
    LIBS += -L$$DEPLOYDIR/lib
  }
  # add CONFIG+=deploy to the qmake command-line to make a deployment build
  deploy {
    message("Building deployment version")
    CONFIG += x86 x86_64
  }
}

include(boost.pri)
include(ghostscript.pri)

INCLUDEPATH += src
OBJECTS_DIR = objects

QMAKE_CXXFLAGS+=-DETLOG
QMAKE_CXXFLAGS+=-DDEBUG=4

LIBS += -lcups


SOURCES = \
          src/tri_logger/tri_logger.cpp

HEADERS = \
          src/tri_logger/tri_logger.hpp \
          src/tri_logger/nullstream.hpp

SOURCES += \
          src/Driver.cpp \
          src/Ctrl-Cut.cpp \
          src/LaserJob.cpp \
          src/util/Eps.cpp \
          src/util/LaserConfig.cpp \
          src/vector/Cut.cpp \
          src/vector/Polyline.cpp \
          src/vector/Vertex.cpp \
          src/vector/Edge.cpp \
          src/vector/HPGLEncoder.cpp \
          src/vector/filters/Deonion.cpp \
          src/vector/filters/Explode.cpp \
          src/vector/filters/Join.cpp

HEADERS += \
          src/Driver.h \
          src/Ctrl-Cut.h \
          src/LaserJob.h \
          src/util/Eps.h \
          src/util/LaserConfig.h \
          src/util/PJL.h \
          src/vector/Cut.h \
          src/vector/Polyline.h \
          src/vector/VTypes.h \
          src/vector/Edge.h \
          src/vector/HPGLEncoder.h \
          src/vector/Vertex.h \
          src/vector/filters/Deonion.h \
          src/vector/filters/Filters.h \
          src/vector/filters/Explode.h \
          src/vector/filters/Join.h
SOURCES += \
          src/raster/DownSample.cpp \
          src/raster/PclEncoder.cpp \
          src/raster/Raster.cpp

HEADERS  += \
          src/raster/DownSample.h \
          src/raster/MMapMatrix.h \
          src/raster/PclEncoder.h \
          src/raster/PPMFile.h \
          src/raster/Raster.h \
          src/raster/RTypes.h
