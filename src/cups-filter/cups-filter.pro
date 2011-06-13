TARGET = ctrl-cut
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
CONFIG += cups
include(common.pri)

DEFINES += ETLOG DEBUG=4

SOURCES += \
	  Driver.cpp \
          Ctrl-Cut.cpp \
          LaserJob.cpp \
          PostscriptParser.cpp \
          util/LaserConfig.cpp \
          util/Eps.cpp \
          tri_logger/tri_logger.cpp \
          vector/HPGLEncoder.cpp \
          vector/CutGraph.cpp \
          vector/CutModel.cpp \
          vector/filters/Explode.cpp \
          vector/filters/Travel.cpp \
          vector/filters/Join.cpp
HEADERS += \
          LaserJob.h \
          FileParser.h \
          util/2D.h \
          util/PJL.h \
          util/Eps.h \
          util/Logger.h \
          util/LaserConfig.h \
          Ctrl-Cut.h \
          Driver.h \
          vector/CutEdge.h \
          vector/CutVertex.h \
          vector/Traverse.h \
          vector/HPGLEncoder.h \
          vector/Geometry.h \
          vector/CutModel.h \
          vector/CutGraph.h \
          vector/filters/Join.h \
          vector/filters/Travel.h \
          vector/filters/Explode.h

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
