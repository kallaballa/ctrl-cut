TARGET = ctrl-cut
CONFIG += boost cups gsapi
include(common.pri)
CONFIG += qt
CONFIG += app_bundle
QT += network

MOC_DIR = objects
UI_DIR = objects
RCC_DIR = objects

macx {
  LIBS += -framework CoreFoundation
  QMAKE_INFO_PLIST = Info.plist
}

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API

INCLUDEPATH += ../cups-filter ../lpd-epilog

FORMS   += CtrlCut.ui LaserDialog.ui

RESOURCES += ctrl-cut.qrc

HEADERS += MainWindow.h \
           EventFilter.h \
           LaserDialog.h \
           NavigationView.h \          
           LpdClient.h \
           StreamUtils.h \
           GroupItem.h \
           CtrlCutScene.h \
           ../cups-filter/vector/geom/Geometry.h \
           ../cups-filter/vector/model/CutModel.h \
	   ../cups-filter/vector/model/Explode.h \
           ../cups-filter/vector/model/Reduce.h \
	   ../cups-filter/vector/graph/Deonion.h \
	   ../cups-filter/vector/graph/CutGraph.h \
           ../cups-filter/vector/graph/SegmentGraph.h \
           ../cups-filter/vector/graph/StringGraph.h \	   
           ../cups-filter/vector/graph/Traverse.h \
	   ../cups-filter/vector/HPGLEncoder.h \
           ../cups-filter/raster/PclEncoder.h \
           ../cups-filter/raster/Dither.h \
           ../cups-filter/util/LaserConfig.h \
           ../cups-filter/util/Eps.h \
           ../cups-filter/FileParser.h \
           ../cups-filter/Driver.h \
           ../cups-filter/LaserJob.h \
           ../cups-filter/tri_logger/tri_logger.hpp \
           ../cups-filter/tri_logger/nullstream.hpp

SOURCES += gui.cpp \
           MainWindow.cpp \
           LaserDialog.cpp \
           NavigationView.cpp \
           LpdClient.cpp \
           StreamUtils.cpp \
           CtrlCutScene.cpp \
	   ../cups-filter/vector/geom/Geometry.cpp \
	   ../cups-filter/vector/model/CutModel.cpp \
           ../cups-filter/vector/model/Explode.cpp \
           ../cups-filter/vector/model/Reduce.cpp \
	   ../cups-filter/vector/graph/Deonion.cpp \
           ../cups-filter/vector/graph/SegmentGraph.cpp \
           ../cups-filter/vector/graph/StringGraph.cpp \
	   ../cups-filter/vector/graph/Traverse.cpp \
           ../cups-filter/vector/HPGLEncoder.cpp \
	   ../cups-filter/raster/PclEncoder.cpp \
           ../cups-filter/raster/Dither.cpp \
           ../cups-filter/raster/DitherFloydSteinberg.cpp \
           ../cups-filter/raster/DitherSierra3.cpp \
           ../cups-filter/raster/DitherBayer.cpp \
           ../cups-filter/raster/DitherJarvis.cpp \
           ../cups-filter/raster/DitherStucki.cpp \
           ../cups-filter/raster/DitherBurke.cpp \
           ../cups-filter/raster/DitherSierra2.cpp \
           ../cups-filter/raster/DitherThreshold.cpp \
           ../cups-filter/util/LaserConfig.cpp \
           ../cups-filter/util/Eps.cpp \
           ../cups-filter/PostscriptParser.cpp \
           ../cups-filter/Driver.cpp \
           ../cups-filter/LaserJob.cpp \
           ../cups-filter/tri_logger/tri_logger.cpp
