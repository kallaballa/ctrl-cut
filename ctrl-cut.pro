TARGET = ctrl-cut
#CONFIG += raster
CONFIG -= qt
macx {
  CONFIG -= app_bundle
  INCLUDEPATH += /opt/local/include 
}

CONFIG += gsapi
gsapi {
  DEFINES += USE_GHOSTSCRIPT_API
  macx {
   INCLUDEPATH += /opt/local/include
   LIBS += -L/opt/local/lib
  }
  LIBS += -lgs
}

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
          src/vector/Renderer.cpp \
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
          src/vector/Renderer.h \
          src/vector/Vertex.h \
          src/vector/filters/Deonion.h \
          src/vector/filters/Filters.h \
          src/vector/filters/Explode.h \
          src/vector/filters/Join.h
SOURCES += \
          src/raster/DownSample.cpp \
          src/raster/PclRenderer.cpp \
          src/raster/Raster.cpp

HEADERS  += \
          src/raster/DownSample.h \
          src/raster/MMapMatrix.h \
          src/raster/PclRenderer.h \
          src/raster/PPMFile.h \
          src/raster/Raster.h \
          src/raster/RTypes.h
