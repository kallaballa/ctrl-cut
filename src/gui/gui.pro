TARGET = gui
CONFIG += boost cups gsapi
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += magick++
CONFIG += X11
CONFIG += libpng
!macx:CONFIG += SDL

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

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API PCLINT_USE_SDL

INCLUDEPATH += ../cups-filter ../lpd-epilog ../pclint

FORMS   += CtrlCut.ui LaserDialog.ui SimulatorDialog.ui

RESOURCES += ctrl-cut.qrc

HEADERS += ./MainWindow.h \
           ./SimulatorDialog.h \
           ./event/CtrlCutEvent.h \
           ./CtrlCutScene.h \
           ./NavigationView.h \
           ./GroupItem.h \
           ./EventFilter.h \
           ./LpdClient.h \
           ./CtrlCutView.h \
           ./LaserDialog.h \
           ./StreamUtils.h \
           ./objects/ui_CtrlCut.h \
           ./objects/ui_SimulatorDialog.h \
           ./objects/ui_LaserDialog.h \
           ./helpers/Qt.h \
           ./helpers/CutItem.h \
           ./helpers/DocumentItem.h \
           ./helpers/EngraveItem.h \
           ./SimulatorView.h \
           ./SimulatorScene.h \
           ../ctrl-cut/encoder/HPGLEncoder.h \
           ../ctrl-cut/encoder/PclEncoder.h \
           ../ctrl-cut/FileParser.h \
           ../ctrl-cut/util/Measurement.h \
           ../ctrl-cut/util/2D.h \
           ../ctrl-cut/util/PJL.h \
           ../ctrl-cut/util/Eps.h \
           ../ctrl-cut/util/Logger.h \
           ../ctrl-cut/util/Util.h \
           ../ctrl-cut/cut/model/CutView.h \
           ../ctrl-cut/cut/model/Reduce.h \
           ../ctrl-cut/cut/model/Factory.h \
           ../ctrl-cut/cut/model/CutModel.h \
           ../ctrl-cut/cut/model/Explode.h \
           ../ctrl-cut/cut/geom/Segment.h \
           ../ctrl-cut/cut/geom/SegmentTree.h \
           ../ctrl-cut/cut/geom/Geometry.h \
           ../ctrl-cut/cut/geom/SegmentString.h \
           ../ctrl-cut/cut/graph/StringGraph.h \
           ../ctrl-cut/cut/graph/Traverse.h \
           ../ctrl-cut/cut/graph/SegmentGraph.h \
           ../ctrl-cut/cut/graph/CutGraph.h \
           ../ctrl-cut/cut/graph/Deonion.h \
           ../ctrl-cut/engrave/Engrave.h \
           ../ctrl-cut/engrave/image/PPMFile.h \
           ../ctrl-cut/engrave/image/MMapMatrix.h \
           ../ctrl-cut/engrave/image/AbstractImage.h \
           ../ctrl-cut/engrave/image/Image.h \
           ../ctrl-cut/engrave/dither/DitherJarvis.h \
           ../ctrl-cut/engrave/dither/DitherBurke.h \
           ../ctrl-cut/engrave/dither/DitherStucki.h \
           ../ctrl-cut/engrave/dither/DitherBayer.h \
           ../ctrl-cut/engrave/dither/DitherSierra2.h \
           ../ctrl-cut/engrave/dither/Dither.h \
           ../ctrl-cut/engrave/dither/DitherFloydSteinberg.h \
           ../ctrl-cut/engrave/dither/DitherThreshold.h \
           ../ctrl-cut/engrave/dither/DitherSierra3.h \
           ../ctrl-cut/Document.h \
           ../ctrl-cut/config/EngraveSettings.h \
           ../ctrl-cut/config/CutSettings.h \
           ../ctrl-cut/config/LaserCutter.h \
           ../ctrl-cut/config/SegmentSettings.h \
           ../ctrl-cut/config/Settings.h \
           ../ctrl-cut/config/DocumentSettings.h \
           ../ctrl-cut/svg/SvgFix.h \
           ../ctrl-cut/svg/SvgDocument.h \
           ../ctrl-cut/svg/Svg2Ps.h \
           ../pclint/Pcl.h \
           ../pclint/CLI.h \
           ../pclint/HPGL.h \
           ../pclint/SDLCanvas.h \
           ../pclint/2D.h \
           ../pclint/Interpreter.h \
           ../pclint/PclIntConfig.h \
           ../pclint/Statistic.h \
           ../pclint/Plot.h \
           ../pclint/Plotter.h \
           ../pclint/Raster.h \
           ../pclint/CImg.h \
           ../pclint/Canvas.h

SOURCES += ./gui.cpp \
           ./event/CtrlCutEvent.cpp \
           ./SimulatorDialog.cpp \
           ./LaserDialog.cpp \
           ./NavigationView.cpp \
           ./CtrlCutView.cpp \
           ./SimulatorView.cpp \
           ./objects/moc_NavigationView.cpp \
           ./objects/moc_SimulatorScene.cpp \
           ./objects/moc_EventFilter.cpp \
           ./objects/moc_EngraveItem.cpp \
           ./objects/moc_MainWindow.cpp \
           ./objects/moc_CtrlCutScene.cpp \
           ./objects/moc_LaserDialog.cpp \
           ./objects/qrc_ctrl-cut.cpp \
           ./objects/moc_DocumentItem.cpp \
           ./objects/moc_SimulatorDialog.cpp \
           ./objects/moc_CutItem.cpp \
           ./objects/moc_LpdClient.cpp \
           ./objects/moc_SimulatorView.cpp \
           ./objects/moc_CtrlCutView.cpp \
           ./objects/moc_Qt.cpp \
           ./helpers/CutItem.cpp \
           ./helpers/Qt.cpp \
           ./helpers/EngraveItem.cpp \
           ./CtrlCutScene.cpp \
           ./LpdClient.cpp \
           ./MainWindow.cpp \
           ./StreamUtils.cpp \
           ./SimulatorScene.cpp \
           ../ctrl-cut/encoder/HPGLEncoder.cpp \
           ../ctrl-cut/encoder/PclEncoder.cpp \
           ../ctrl-cut/PostscriptParser.cpp \
           ../ctrl-cut/util/Measurement.cpp \
           ../ctrl-cut/util/Logger.cpp \
           ../ctrl-cut/util/Eps.cpp \
           ../ctrl-cut/cut/model/CutView.cpp \
           ../ctrl-cut/cut/model/Explode.cpp \
           ../ctrl-cut/cut/model/CutModel.cpp \
           ../ctrl-cut/cut/model/Factory.cpp \
           ../ctrl-cut/cut/model/Reduce.cpp \
           ../ctrl-cut/cut/geom/Geometry.cpp \
           ../ctrl-cut/cut/geom/Segment.cpp \
           ../ctrl-cut/cut/graph/SegmentGraph.cpp \
           ../ctrl-cut/cut/graph/Traverse.cpp \
           ../ctrl-cut/cut/graph/Deonion.cpp \
           ../ctrl-cut/cut/graph/StringGraph.cpp \
           ../ctrl-cut/engrave/Engrave.cpp \
           ../ctrl-cut/engrave/dither/DitherBayer.cpp \
           ../ctrl-cut/engrave/dither/DitherFloydSteinberg.cpp \
           ../ctrl-cut/engrave/dither/DitherBurke.cpp \
           ../ctrl-cut/engrave/dither/DitherThreshold.cpp \
           ../ctrl-cut/engrave/dither/DitherSierra2.cpp \
           ../ctrl-cut/engrave/dither/DitherStucki.cpp \
           ../ctrl-cut/engrave/dither/DitherJarvis.cpp \
           ../ctrl-cut/engrave/dither/Dither.cpp \
           ../ctrl-cut/engrave/dither/DitherSierra3.cpp \
           ../ctrl-cut/Document.cpp \
           ../ctrl-cut/tri_logger/tri_logger.cpp \
           ../ctrl-cut/config/EngraveSettings.cpp \
           ../ctrl-cut/config/SegmentSettings.cpp \
           ../ctrl-cut/config/CutSettings.cpp \
           ../ctrl-cut/config/DocumentSettings.cpp \
           ../ctrl-cut/svg/SvgFix.cpp \
           ../ctrl-cut/svg/SvgDocument.cpp \
           ../ctrl-cut/svg/Svg2Ps.cpp \
           ../pclint/SDLCanvas.cpp \
           ../pclint/Canvas.cpp \
           ../pclint/pclint.cpp