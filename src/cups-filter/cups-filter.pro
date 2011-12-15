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
           ../ctrl-cut/cut/HPGLEncoder.cpp \
           ../ctrl-cut/cut/model/Explode.cpp \
           ../ctrl-cut/cut/model/CutModel.cpp \
           ../ctrl-cut/cut/model/Reduce.cpp \
           ../ctrl-cut/cut/geom/Geometry.cpp \
           ../ctrl-cut/cut/graph/SegmentGraph.cpp \
           ../ctrl-cut/cut/graph/Traverse.cpp \
           ../ctrl-cut/cut/graph/Deonion.cpp \
           ../ctrl-cut/cut/graph/StringGraph.cpp \
           ../ctrl-cut/engrave/DitherBayer.cpp \
           ../ctrl-cut/engrave/DitherFloydSteinberg.cpp \
           ../ctrl-cut/engrave/DitherBurke.cpp \
           ../ctrl-cut/engrave/DitherThreshold.cpp \
           ../ctrl-cut/engrave/DitherSierra2.cpp \
           ../ctrl-cut/engrave/PclEncoder.cpp \
           ../ctrl-cut/engrave/Engrave.cpp \
           ../ctrl-cut/engrave/DitherStucki.cpp \
           ../ctrl-cut/engrave/DitherJarvis.cpp \
           ../ctrl-cut/engrave/Dither.cpp \
           ../ctrl-cut/engrave/DitherSierra3.cpp \
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
           ../ctrl-cut/cut/HPGLEncoder.h \
           ../ctrl-cut/cut/graph/StringGraph.h \
           ../ctrl-cut/cut/graph/Traverse.h \
           ../ctrl-cut/cut/graph/SegmentGraph.h \
           ../ctrl-cut/cut/graph/CutGraph.h \
           ../ctrl-cut/cut/graph/Deonion.h \
           ../ctrl-cut/engrave/PPMFile.h \
           ../ctrl-cut/engrave/DitherJarvis.h \
           ../ctrl-cut/engrave/DitherBurke.h \
           ../ctrl-cut/engrave/Engrave.h \
           ../ctrl-cut/engrave/DitherStucki.h \
           ../ctrl-cut/engrave/DitherBayer.h \
           ../ctrl-cut/engrave/DitherSierra2.h \
           ../ctrl-cut/engrave/image/MMapMatrix.h \
           ../ctrl-cut/engrave/image/AbstractImage.h \
           ../ctrl-cut/engrave/image/Image.h \
           ../ctrl-cut/engrave/Dither.h \
           ../ctrl-cut/engrave/DitherFloydSteinberg.h \
           ../ctrl-cut/engrave/DitherThreshold.h \
           ../ctrl-cut/engrave/PclEncoder.h \
           ../ctrl-cut/engrave/DitherSierra3.h \
           ../ctrl-cut/Document.h \
           ../ctrl-cut/config/EngraveSettings.h \
           ../ctrl-cut/config/CutSettings.h \
           ../ctrl-cut/config/LaserCutter.h \
           ../ctrl-cut/config/Settings.h \
           ../ctrl-cut/config/DocumentSettings.h \
           ../ctrl-cut/svg/SvgFix.h \
           ../ctrl-cut/svg/SvgDocument.h \
           ../ctrl-cut/svg/Svg2Ps.h
