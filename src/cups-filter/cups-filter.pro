TARGET = cups-filter
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

INCLUDEPATH += ../ctrl-cut/

include(common.pri)

DEFINES += ETLOG DEBUG=4

SOURCES += ./CupsGetOpt.cpp \
           ./CupsFilter.cpp \
           ./CupsOptions.cpp \
           ../ctrl-cut/PostscriptParser.cpp \
           ../ctrl-cut/util/Measurement.cpp \
           ../ctrl-cut/util/Logger.cpp \
           ../ctrl-cut/util/Eps.cpp \
           ../ctrl-cut/encoder/HPGLEncoder.cpp \
           ../ctrl-cut/cut/model/Explode.cpp \
           ../ctrl-cut/cut/model/CutModel.cpp \
           ../ctrl-cut/cut/model/Reduce.cpp \
           ../ctrl-cut/cut/geom/Geometry.cpp \
           ../ctrl-cut/cut/graph/SegmentGraph.cpp \
           ../ctrl-cut/cut/graph/Traverse.cpp \
           ../ctrl-cut/cut/graph/Deonion.cpp \
           ../ctrl-cut/cut/graph/StringGraph.cpp \
           ../ctrl-cut/engrave/dither/DitherBayer.cpp \
           ../ctrl-cut/engrave/dither/DitherFloydSteinberg.cpp \
           ../ctrl-cut/engrave/dither/DitherBurke.cpp \
           ../ctrl-cut/engrave/dither/DitherThreshold.cpp \
           ../ctrl-cut/engrave/dither/DitherSierra2.cpp \
           ../ctrl-cut/encoder/PclEncoder.cpp \
           ../ctrl-cut/engrave/Engrave.cpp \
           ../ctrl-cut/engrave/dither/DitherStucki.cpp \
           ../ctrl-cut/engrave/dither/DitherJarvis.cpp \
           ../ctrl-cut/engrave/dither/Dither.cpp \
           ../ctrl-cut/engrave/dither/DitherSierra3.cpp \
           ../ctrl-cut/Document.cpp \
           ../ctrl-cut/tri_logger/tri_logger.cpp \
           ../ctrl-cut/config/EngraveSettings.cpp \
           ../ctrl-cut/config/CutSettings.cpp \
           ../ctrl-cut/config/DocumentSettings.cpp \
           ../ctrl-cut/svg/SvgFix.cpp \
           ../ctrl-cut/svg/SvgDocument.cpp \
           ../ctrl-cut/svg/Svg2Ps.cpp

HEADERS += \
           ./CupsGetOpt.h \
           ./CupsOptions.h \
           ../ctrl-cut/FileParser.h \
           ../ctrl-cut/util/Measurement.h \
           ../ctrl-cut/util/2D.h \
           ../ctrl-cut/util/PJL.h \
           ../ctrl-cut/util/Eps.h \
           ../ctrl-cut/util/Logger.h \
           ../ctrl-cut/util/Util.h \
           ../ctrl-cut/cut/model/Reduce.h \
           ../ctrl-cut/cut/model/CutModel.h \
           ../ctrl-cut/cut/model/Explode.h \
           ../ctrl-cut/cut/geom/SegmentTree.h \
           ../ctrl-cut/cut/geom/Geometry.h \
           ../ctrl-cut/encoder/HPGLEncoder.h \
           ../ctrl-cut/cut/graph/StringGraph.h \
           ../ctrl-cut/cut/graph/Traverse.h \
           ../ctrl-cut/cut/graph/SegmentGraph.h \
           ../ctrl-cut/cut/graph/CutGraph.h \
           ../ctrl-cut/cut/graph/Deonion.h \
           ../ctrl-cut/engrave/image/PPMFile.h \
           ../ctrl-cut/engrave/dither/DitherJarvis.h \
           ../ctrl-cut/engrave/dither/DitherBurke.h \
           ../ctrl-cut/engrave/Engrave.h \
           ../ctrl-cut/engrave/dither/DitherStucki.h \
           ../ctrl-cut/engrave/dither/DitherBayer.h \
           ../ctrl-cut/engrave/dither/DitherSierra2.h \
           ../ctrl-cut/engrave/image/MMapMatrix.h \
           ../ctrl-cut/engrave/image/AbstractImage.h \
           ../ctrl-cut/engrave/image/Image.h \
           ../ctrl-cut/engrave/dither/Dither.h \
           ../ctrl-cut/engrave/dither/DitherFloydSteinberg.h \
           ../ctrl-cut/engrave/dither/DitherThreshold.h \
           ../ctrl-cut/engrave/image/PclEncoder.h \
           ../ctrl-cut/engrave/dither/DitherSierra3.h \
           ../ctrl-cut/Document.h \
           ../ctrl-cut/config/EngraveSettings.h \
           ../ctrl-cut/config/CutSettings.h \
           ../ctrl-cut/config/LaserCutter.h \
           ../ctrl-cut/config/Settings.h \
           ../ctrl-cut/config/DocumentSettings.h \
           ../ctrl-cut/svg/SvgFix.h \
           ../ctrl-cut/svg/SvgDocument.h \
           ../ctrl-cut/svg/Svg2Ps.h
