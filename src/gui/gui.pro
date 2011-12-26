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
CONFIG += libctrl-cut
!macx:CONFIG += SDL

include(common.pri)
CONFIG += qt
CONFIG += app_bundle
QT += network

MOC_DIR = objects
UI_DIR = objects
RCC_DIR = objects

macx {
  TARGET = gui
  ICON = icons/Ctrl-Cut.icns
  QMAKE_INFO_PLIST = Info.plist
  LIBS += -framework CoreFoundation
}

DEFINES += ETLOG DEBUG=4 USE_GHOSTSCRIPT_API PCLINT_USE_SDL

INCLUDEPATH += ../lpd-epilog ../pclint

FORMS += CtrlCut.ui LaserDialog.ui SimulatorDialog.ui

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
           ./helpers/Qt.h \
           ./helpers/CutItem.h \
           ./helpers/DocumentItem.h \
           ./helpers/EngraveItem.h \
           ./SimulatorView.h \
           ./SimulatorScene.h \
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
           ./helpers/CutItem.cpp \
           ./helpers/Qt.cpp \
           ./helpers/EngraveItem.cpp \
           ./CtrlCutScene.cpp \
           ./LpdClient.cpp \
           ./MainWindow.cpp \
           ./StreamUtils.cpp \
           ./SimulatorScene.cpp \
           ../pclint/SDLCanvas.cpp \
           ../pclint/Canvas.cpp \
           ../pclint/pclint.cpp
