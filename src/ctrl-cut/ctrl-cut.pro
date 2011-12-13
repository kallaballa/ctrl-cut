TARGET = ctrl-cut
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
CONFIG += cups
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += magick++

include(common.pri)

DEFINES += ETLOG DEBUG=4

SOURCES += \
          Document.cpp \
          Driver.cpp \
          PostscriptParser.cpp \
          config/CutSettings.cpp \
          config/DocumentSettings.cpp \
          config/EngraveSettings.cpp \
          config/Settings.cpp \
          svg/Svg2Ps.cpp \
          svg/SvgDocument.cpp \
          svg/SvgFix.cpp \
          tri_logger/tri_logger.cpp \
          util/Eps.cpp \
          util/Logger.cpp \
          util/Measurement.cpp \
          vector/HPGLEncoder.cpp \
          vector/geom/Geometry.cpp \
          vector/graph/Deonion.cpp \
          vector/graph/SegmentGraph.cpp \
          vector/graph/StringGraph.cpp \
          vector/graph/Traverse.cpp \
          vector/model/CutModel.cpp \
          vector/model/Explode.cpp \
          vector/model/Reduce.cpp

HEADERS += \
          Document.h \
          Driver.h \
          FileParser.h \
          config/CutSettings.h \
          config/DocumentSettings.h \
          config/EngraveSettings.h \
          config/LaserCutter.h \
          config/Settings.h \
          config/Trigger.h \
          svg/Svg2Ps.h \
          svg/SvgDocument.h \
          svg/SvgFix.h \
          tri_logger/nullstream.hpp \
          tri_logger/tri_logger.hpp \
          util/2D.h \
          util/Eps.h \
          util/Logger.h \
          util/Measurement.h \
          util/PJL.h \
          util/Util.h \
          vector/HPGLEncoder.h \
          vector/geom/Geometry.h \
          vector/geom/SegmentTree.h \
          vector/graph/CutGraph.h \
          vector/graph/Deonion.cpp \
          vector/graph/Deonion.h \
          vector/graph/SegmentGraph.cpp \
          vector/graph/SegmentGraph.h \
          vector/graph/StringGraph.cpp \
          vector/graph/StringGraph.h \
          vector/graph/Traverse.cpp \
          vector/graph/Traverse.h \
          vector/model/CutModel.h \
          vector/model/Explode.h \
          vector/model/Reduce.h

SOURCES += \
          raster/Dither.cpp \
          raster/DitherBayer.cpp \
          raster/DitherBurke.cpp \
          raster/DitherFloydSteinberg.cpp \
          raster/DitherJarvis.cpp \
          raster/DitherSierra2.cpp \
          raster/DitherSierra3.cpp \
          raster/DitherStucki.cpp \
          raster/DitherThreshold.cpp \
          raster/DownSample.cpp \
          raster/PclEncoder.cpp \
          raster/Raster.cpp
HEADERS  += \
          raster/AbstractImage.h \
          raster/Dither.cpp \
          raster/Dither.h \
          raster/DitherBayer.cpp \
          raster/DitherBayer.h \
          raster/DitherBurke.cpp \
          raster/DitherBurke.h \
          raster/DitherFloydSteinberg.cpp \
          raster/DitherFloydSteinberg.h \
          raster/DitherJarvis.cpp \
          raster/DitherJarvis.h \
          raster/DitherSierra2.cpp \
          raster/DitherSierra2.h \
          raster/DitherSierra3.cpp \
          raster/DitherSierra3.h \
          raster/DitherStucki.cpp \
          raster/DitherStucki.h \
          raster/DitherThreshold.cpp \
          raster/DitherThreshold.h \
          raster/DownSample.h \
          raster/Image.h \
          raster/MMapMatrix.h \
          raster/PPMFile.h \
          raster/PclEncoder.h \
          raster/Raster.h
