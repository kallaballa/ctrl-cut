TARGET = gui
CONFIG += boost cups gsapi
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += magick++

INCLUDEPATH += ../ctrl-cut/

include(common.pri)
CONFIG += qt
CONFIG += app_bundle
QT += network

MOC_DIR = objects
UI_DIR = objects
RCC_DIR = objects

macx {
  TARGET = Ctrl-Cut
  ICON = icons/Ctrl-Cut.icns
  QMAKE_INFO_PLIST = Info.plist
  LIBS += -framework CoreFoundation
}

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API

INCLUDEPATH += ../cups-filter ../lpd-epilog

FORMS   += CtrlCut.ui LaserDialog.ui

RESOURCES += ctrl-cut.qrc

HEADERS += MainWindow.h \
           EventFilter.h \
           LaserDialog.h \
           NavigationView.h \          
           CtrlCutView.h \          
           LpdClient.h \
           StreamUtils.h \
           GroupItem.h \
           CtrlCutScene.h \
          ../ctrl-cut/Document.h \
          ../ctrl-cut/Driver.h \
          ../ctrl-cut/FileParser.h \
          ../ctrl-cut/config/CutSettings.h \
          ../ctrl-cut/config/DocumentSettings.h \
          ../ctrl-cut/config/EngraveSettings.h \
          ../ctrl-cut/config/LaserCutter.h \
          ../ctrl-cut/config/Settings.h \
          ../ctrl-cut/config/Trigger.h \
          ../ctrl-cut/svg/Svg2Ps.h \
          ../ctrl-cut/svg/SvgDocument.h \
          ../ctrl-cut/svg/SvgFix.h \
          ../ctrl-cut/tri_logger/nullstream.hpp \
          ../ctrl-cut/tri_logger/tri_logger.hpp \
          ../ctrl-cut/util/2D.h \
          ../ctrl-cut/util/Eps.h \
          ../ctrl-cut/util/Logger.h \
          ../ctrl-cut/util/Measurement.h \
          ../ctrl-cut/util/PJL.h \
          ../ctrl-cut/util/Util.h \
          ../ctrl-cut/vector/HPGLEncoder.h \
          ../ctrl-cut/vector/geom/Geometry.h \
          ../ctrl-cut/vector/geom/SegmentTree.h \
          ../ctrl-cut/vector/graph/CutGraph.h \
          ../ctrl-cut/vector/graph/Deonion.cpp \
          ../ctrl-cut/vector/graph/Deonion.h \
          ../ctrl-cut/vector/graph/SegmentGraph.cpp \
          ../ctrl-cut/vector/graph/SegmentGraph.h \
          ../ctrl-cut/vector/graph/StringGraph.cpp \
          ../ctrl-cut/vector/graph/StringGraph.h \
          ../ctrl-cut/vector/graph/Traverse.cpp \
          ../ctrl-cut/vector/graph/Traverse.h \
          ../ctrl-cut/vector/model/CutModel.h \
          ../ctrl-cut/vector/model/Explode.h \
          ../ctrl-cut/vector/model/Reduce.h \
          ../ctrl-cut/raster/AbstractImage.h \
          ../ctrl-cut/raster/Dither.cpp \
          ../ctrl-cut/raster/Dither.h \
          ../ctrl-cut/raster/DitherBayer.cpp \
          ../ctrl-cut/raster/DitherBayer.h \
          ../ctrl-cut/raster/DitherBurke.cpp \
          ../ctrl-cut/raster/DitherBurke.h \
          ../ctrl-cut/raster/DitherFloydSteinberg.cpp \
          ../ctrl-cut/raster/DitherFloydSteinberg.h \
          ../ctrl-cut/raster/DitherJarvis.cpp \
          ../ctrl-cut/raster/DitherJarvis.h \
          ../ctrl-cut/raster/DitherSierra2.cpp \
          ../ctrl-cut/raster/DitherSierra2.h \
          ../ctrl-cut/raster/DitherSierra3.cpp \
          ../ctrl-cut/raster/DitherSierra3.h \
          ../ctrl-cut/raster/DitherStucki.cpp \
          ../ctrl-cut/raster/DitherStucki.h \
          ../ctrl-cut/raster/DitherThreshold.cpp \
          ../ctrl-cut/raster/DitherThreshold.h \
          ../ctrl-cut/raster/DownSample.h \
          ../ctrl-cut/raster/Image.h \
          ../ctrl-cut/raster/MMapMatrix.h \
          ../ctrl-cut/raster/PPMFile.h \
          ../ctrl-cut/raster/PclEncoder.h \
          ../ctrl-cut/raster/Raster.h

SOURCES += gui.cpp \
           MainWindow.cpp \
           LaserDialog.cpp \
           NavigationView.cpp \
           CtrlCutView.cpp \
           LpdClient.cpp \
           StreamUtils.cpp \
           CtrlCutScene.cpp \
          ../ctrl-cut/Document.cpp \
          ../ctrl-cut/Driver.cpp \
          ../ctrl-cut/PostscriptParser.cpp \
          ../ctrl-cut/config/CutSettings.cpp \
          ../ctrl-cut/config/DocumentSettings.cpp \
          ../ctrl-cut/config/EngraveSettings.cpp \
          ../ctrl-cut/config/Settings.cpp \
          ../ctrl-cut/svg/Svg2Ps.cpp \
          ../ctrl-cut/svg/SvgDocument.cpp \
          ../ctrl-cut/svg/SvgFix.cpp \
          ../ctrl-cut/tri_logger/tri_logger.cpp \
          ../ctrl-cut/util/Eps.cpp \
          ../ctrl-cut/util/Logger.cpp \
          ../ctrl-cut/util/Measurement.cpp \
          ../ctrl-cut/vector/HPGLEncoder.cpp \
          ../ctrl-cut/vector/geom/Geometry.cpp \
          ../ctrl-cut/vector/graph/Deonion.cpp \
          ../ctrl-cut/vector/graph/SegmentGraph.cpp \
          ../ctrl-cut/vector/graph/StringGraph.cpp \
          ../ctrl-cut/vector/graph/Traverse.cpp \
          ../ctrl-cut/vector/model/CutModel.cpp \
          ../ctrl-cut/vector/model/Explode.cpp \
          ../ctrl-cut/vector/model/Reduce.cpp \
          ../ctrl-cut/raster/Dither.cpp \
          ../ctrl-cut/raster/DitherBayer.cpp \
          ../ctrl-cut/raster/DitherBurke.cpp \
          ../ctrl-cut/raster/DitherFloydSteinberg.cpp \
          ../ctrl-cut/raster/DitherJarvis.cpp \
          ../ctrl-cut/raster/DitherSierra2.cpp \
          ../ctrl-cut/raster/DitherSierra3.cpp \
          ../ctrl-cut/raster/DitherStucki.cpp \
          ../ctrl-cut/raster/DitherThreshold.cpp \
          ../ctrl-cut/raster/DownSample.cpp \
          ../ctrl-cut/raster/PclEncoder.cpp \
          ../ctrl-cut/raster/Raster.cpp
