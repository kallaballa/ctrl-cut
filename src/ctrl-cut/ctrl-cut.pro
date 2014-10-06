TEMPLATE = lib
CONFIG += dll
CONFIG += boost cups gsapi
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libpng
CONFIG += libxml++
CONFIG += magick++
QMAKE_CXXFLAGS += -fPIC
INCLUDEPATH += 

include(common.pri)

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API

HEADERS += ./cutters/encoder/PclEncoder.hpp \
           ./cutters/encoder/HPGLEncoder.hpp \
           ./FileParser.hpp \
           ./Document.hpp \
           ./util/Eps.hpp \
           ./util/2D.hpp \
           ./util/Measurement.hpp \
           ./util/Logger.hpp \
           ./util/PJL.hpp \
           ./util/Util.hpp \
           ./cutters/EpilogLegend36Ext.hpp \
           ./cut/operations/Explode.hpp \
           ./cut/operations/Reduce.hpp \
	   			 ./cut/Cut.hpp \
           ./cut/operations/SvgPlot.hpp \
           ./cut/operations/Deonion.hpp \
           ./cut/operations/NearestPathSorting.hpp \
           ./cut/operations/Traveller.hpp \
           ./cut/operations/Clip.cpp \
           ./cut/operations/Planar.hpp \
          ./cut/geom/algorithms/Algorithms.hpp \
           ./cut/geom/algorithms/Add.hpp \
           ./cut/geom/algorithms/Concat.hpp \
           ./cut/geom/algorithms/Append.hpp \
           ./cut/geom/algorithms/Prepend.hpp \
           ./cut/geom/Views.hpp \
           ./cut/geom/Geometry.hpp \
           ./cut/geom/SegmentTree.hpp \
           ./cut/geom/Path.hpp \
           ./cut/geom/Iterators.hpp \
           ./cut/geom/Route.hpp \
           ./cut/geom/Concepts.hpp \
           ./cut/graph/SegmentGraph.hpp \
           ./cut/graph/GeometryGraph.hpp \
           ./cut/graph/CompleteDistanceGraph.hpp \
           ./cut/graph/Traverse.hpp \
           ./cut/graph/Algorithms.hpp \
           ./engrave/image/PPMFile.hpp \
           ./engrave/image/Image.hpp \
           ./engrave/image/AbstractImage.hpp \
           ./engrave/image/MMapMatrix.hpp \
           ./engrave/Engrave.hpp \
           ./engrave/dither/DitherJarvis.hpp \
           ./engrave/dither/DitherSierra2.hpp \
           ./engrave/dither/DitherBurke.hpp \
           ./engrave/dither/DitherStucki.hpp \
           ./engrave/dither/DitherBayer.hpp \
           ./engrave/dither/DitherFloydSteinberg.hpp \
           ./engrave/dither/DitherThreshold.hpp \
           ./engrave/dither/Dither.hpp \
           ./engrave/dither/DitherSierra3.hpp \
           ./tri_logger/nullstream.hpp \
           ./tri_logger/tri_logger.hpp \
           ./config/CutSettings.hpp \
           ./config/DocumentSettings.hpp \
           ./config/LaserCutter.hpp \
           ./config/Settings.hpp \
           ./config/EngraveSettings.hpp \
           ./svg/Svg2Ps.hpp \
           ./svg/SvgDocument.hpp \
           ./svg/SvgFix.hpp \
	   ./svg/CtrlCutParser.hpp

SOURCES += ./cutters/encoder/HPGLEncoder.cpp \
           ./cutters/encoder/PclEncoder.cpp \
           ./PostscriptParser.cpp \
           ./util/2D.cpp \
           ./util/Measurement.cpp \
           ./util/Logger.cpp \
           ./util/Eps.cpp \
	   ./cutters/EpilogLegend36Ext.cpp \
           ./cut/operations/Deonion.cpp \
           ./cut/operations/NearestPathSorting.cpp \
           ./cut/operations/Traveller.cpp \
           ./cut/operations/Planar.cpp \
           ./cut/operations/Explode.cpp \
           ./cut/operations/Clip.cpp \
           ./cut/operations/Reduce.cpp \
           ./cut/operations/Translate.cpp \
           ./svg/SvgPlot.cpp \
           ./engrave/dither/DitherBayer.cpp \
           ./engrave/dither/DitherFloydSteinberg.cpp \
           ./engrave/dither/DitherBurke.cpp \
           ./engrave/dither/DitherThreshold.cpp \
           ./engrave/dither/DitherSierra2.cpp \
           ./engrave/dither/DitherStucki.cpp \
           ./engrave/dither/DitherJarvis.cpp \
           ./engrave/dither/Dither.cpp \
           ./engrave/dither/DitherSierra3.cpp \
           ./Document.cpp \
           ./tri_logger/tri_logger.cpp \
           ./config/EngraveSettings.cpp \
           ./config/CutSettings.cpp \
           ./config/DocumentSettings.cpp \
	   ./svg/SvgWriter.cpp \  
           ./svg/SvgFix.cpp \
           ./svg/SvgDocument.cpp \
           ./svg/Svg2Ps.cpp \
           ./svg/CtrlCutParser.cpp

