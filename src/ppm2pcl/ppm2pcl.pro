TARGET = ppm2pcl
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
include(common.pri)

QMAKE_CXXFLAGS+=-DETLOG
QMAKE_CXXFLAGS+=-DDEBUG=4

LIBS += -lcups

SOURCES += \
          Driver.cpp \
          ppm2pcl.cpp \
          LaserJob.cpp \
          util/Eps.cpp \
          util/LaserConfig.cpp \
          vector/Cut.cpp \
          vector/Mesh.cpp \
          vector/Polyline.cpp \
          vector/Vertex.cpp \
          vector/Edge.cpp \
          vector/HPGLEncoder.cpp \
          vector/filters/Deonion.cpp \
          vector/filters/Explode.cpp \
          vector/filters/Join.cpp \
          vector/filters/Flat.cpp \
          tri_logger/tri_logger.cpp

HEADERS += \
          Driver.h \
          LaserJob.h \
          util/Eps.h \
          util/LaserConfig.h \
          util/PJL.h \
          util/2D.h \
          vector/Cut.h \
          vector/Mesh.h \
          vector/Polyline.h \
          vector/Edge.h \
          vector/HPGLEncoder.h \
          vector/Vertex.h \
          vector/filters/Deonion.h \
          vector/filters/Filters.h \
          vector/filters/Explode.h \
          vector/filters/Join.h \
          vector/filters/Flat.h \
          tri_logger/tri_logger.hpp \
          tri_logger/nullstream.hpp

SOURCES += \
          raster/DownSample.cpp \
          raster/PclEncoder.cpp \
          raster/Raster.cpp

HEADERS  += \
          raster/DownSample.h \
          raster/MMapMatrix.h \
          raster/PclEncoder.h \
          raster/PPMFile.h \
          raster/Raster.h \

