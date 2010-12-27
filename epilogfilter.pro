TARGET = EpilogFilter
#CONFIG += raster
CONFIG -= qt
macx {
  CONFIG -= app_bundle
  INCLUDEPATH += /opt/local/include 
}

OBJECTS_DIR = objects

DEFINES += cimg_display=0

LIBS += -lcups

SOURCES = \
          src/Driver.cpp \
          src/EpilogFilter.cpp \
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

HEADERS = \
          src/Driver.h \
          src/EpilogFilter.h \
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

raster {
SOURCES += \
          src/raster/DownSample.cpp \
          src/raster/filter/TAFilter.cpp \
          src/raster/filter/TPFilter.cpp \
          src/raster/MMapImage.cpp \
          src/raster/PclRenderer.cpp \
          src/raster/PPMFile.cpp \
          src/raster/Raster.cpp

HEADERS  += \
          src/raster/DownSample.h \
          src/raster/filter/TAFilter.h \
          src/raster/filter/TPFilter.h \
          src/raster/MMapImage.h \
          src/raster/PclRenderer.h \
          src/raster/PPMFile.h \
          src/raster/Raster.h \
          src/raster/RTypes.h
}
