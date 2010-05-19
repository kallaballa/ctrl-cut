TARGET = EpilogFilter
macx {
  CONFIG -= app_bundle
  INCLUDEPATH += /opt/local/include 
}

OBJECTS_DIR = objects

DEFINES += cimg_display=0

LIBS += -lcups

SOURCES = \
          src/DeonionFilter.cpp \
          src/Driver.cpp \
          src/EpilogFilter.cpp \
          src/FilterChain.cpp \
          src/FindPolylinesFilter.cpp \
          src/Joint.cpp \
          src/LaserJob.cpp \
          src/LineSegment.cpp \
          src/OnionSkin.cpp \
          src/Polygon.cpp \
          src/Polyline.cpp \
          src/RasterPass.cpp \
          src/Filter.cpp \
          src/SerializeOnionSkinsFilter.cpp \
          src/SplitIntersectionsFilter.cpp \
          src/TAFilter.cpp \
          src/TilePartitioner.cpp \
          src/TPFilter.cpp \
          src/VectorPass.cpp \
          src/laser_config.cpp \
          src/eps_converter.cpp

HEADERS = \
          src/CImg.h \
          src/DeonionFilter.h \
          src/Driver.h \
          src/EpilogFilter.h \
          src/Filter.h \
          src/FilterChain.h \
          src/FindPolylinesFilter.h \
          src/Joint.h \
          src/Laser.h \
          src/laser_config.h \
          src/eps_converter.h \
          src/LaserJob.h \
          src/LaserPass.h \
          src/VectorPass.h \
          src/RasterPass.h \
          src/LineSegment.h \
          src/OnionSkin.h \
          src/pjl.h \
          src/Polygon.h \
          src/Polyline.h \
          src/Primitives.h \
          src/Raster.h \
          src/SerializeOnionSkinsFilter.h \
          src/SplitIntersectionsFilter.h \
          src/TAFilter.h \
          src/TilePartitioner.h \
          src/TPFilter.h
