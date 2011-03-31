TARGET = ctrl-cut
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
include(common.pri)

QMAKE_CXXFLAGS+=-DETLOG
QMAKE_CXXFLAGS+=-DDEBUG=4

LIBS += -lcups

SOURCES += \
          Driver.cpp \
          Ctrl-Cut.cpp \
          LaserJob.cpp \
          PostscriptParser.cpp \
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
          vector/filters/Reduce.cpp \
          tri_logger/tri_logger.cpp

HEADERS += \
          Driver.h \
          Ctrl-Cut.h \
          LaserJob.h \
          FileParser.h \
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
          vector/filters/Reduce.h \
          tri_logger/tri_logger.hpp \
          tri_logger/nullstream.hpp

SOURCES += \
          raster/DownSample.cpp \
          raster/PclEncoder.cpp \
          raster/Raster.cpp \
          raster/Dither.cpp \
          raster/DitherFloydSteinberg.cpp \
          raster/DitherSierra3.cpp \
          raster/DitherBayer.cpp \
          raster/DitherJarvis.cpp \
          raster/DitherStucki.cpp \
          raster/DitherBurke.cpp \
          raster/DitherSierra2.cpp \
          raster/DitherThreshold.cpp
          

HEADERS  += \
          raster/DownSample.h \
          raster/AbstractImage.h \
          raster/Image.h \
          raster/MMapMatrix.h \
          raster/PclEncoder.h \
          raster/PPMFile.h \
          raster/Raster.h \
          raster/Dither.h \
          raster/DitherFloydSteinberg.h \
          raster/DitherSierra3.h \
          raster/DitherBayer.h \
          raster/DitherJarvis.h \
          raster/DitherStucki.h \
          raster/DitherBurke.h \
          raster/DitherSierra2.h \
          raster/DitherThreshold.h
          
