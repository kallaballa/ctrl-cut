TEMLATE = lib
TARGET = libctrl-cut.so
CONFIG += dll
CONFIG += boost cups gsapi
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += magick++
QMAKE_LFLAGS += -shared -Wl,-soname,libctrl-cut.so
QMAKE_CXXFLAGS += -fPIC

INCLUDEPATH += 

include(common.pri)

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API

HEADERS += ./encoder/HPGLEncoder.h \
           ./encoder/PclEncoder.h \
           ./FileParser.h \
           ./util/Measurement.h \
           ./util/2D.h \
           ./util/PJL.h \
           ./util/Eps.h \
           ./util/Logger.h \
           ./util/Util.h \
           ./cut/model/Reduce.h \
           ./cut/model/CutModel.h \
           ./cut/model/Explode.h \
           ./cut/geom/Views.h \
           ./cut/geom/Segment.h \
           ./cut/geom/Route.h \
           ./cut/geom/SegmentTree.h \
           ./cut/geom/Geometry.h \
           ./cut/geom/LineString.h \
           ./cut/graph/StringGraph.h \
           ./cut/graph/Traverse.h \
           ./cut/graph/SegmentGraph.h \
           ./cut/graph/CutGraph.h \
           ./cut/graph/Deonion.h \
           ./engrave/Engrave.h \
           ./engrave/image/PPMFile.h \
           ./engrave/image/MMapMatrix.h \
           ./engrave/image/AbstractImage.h \
           ./engrave/image/Image.h \
           ./engrave/dither/DitherJarvis.h \
           ./engrave/dither/DitherBurke.h \
           ./engrave/dither/DitherStucki.h \
           ./engrave/dither/DitherBayer.h \
           ./engrave/dither/DitherSierra2.h \
           ./engrave/dither/Dither.h \
           ./engrave/dither/DitherFloydSteinberg.h \
           ./engrave/dither/DitherThreshold.h \
           ./engrave/dither/DitherSierra3.h \
           ./Document.h \
           ./config/EngraveSettings.h \
           ./config/CutSettings.h \
           ./config/LaserCutter.h \
           ./config/SegmentSettings.h \
           ./config/Settings.h \
           ./config/DocumentSettings.h \
           ./svg/SvgFix.h \
           ./svg/SvgDocument.h \
           ./svg/Svg2Ps.h
SOURCES += ./encoder/HPGLEncoder.cpp \
           ./encoder/PclEncoder.cpp \
           ./PostscriptParser.cpp \
           ./util/Measurement.cpp \
           ./util/Logger.cpp \
           ./util/Eps.cpp \
           ./cut/model/Explode.cpp \
           ./cut/model/Reduce.cpp \
           ./cut/geom/Geometry.cpp \
           ./cut/geom/Route.cpp \
           ./cut/geom/Segment.cpp \
           ./cut/graph/SegmentGraph.cpp \
           ./cut/graph/Traverse.cpp \
           ./cut/graph/Deonion.cpp \
           ./cut/graph/StringGraph.cpp \
           ./engrave/Engrave.cpp \
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
           ./config/SegmentSettings.cpp \
           ./config/CutSettings.cpp \
           ./config/DocumentSettings.cpp \
           ./svg/SvgFix.cpp \
           ./svg/SvgDocument.cpp \
           ./svg/Svg2Ps.cpp