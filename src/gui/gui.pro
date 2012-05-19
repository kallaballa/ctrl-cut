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
CONFIG += SDL

# Prefer our files over system includes which happens to have the same filenames
INCLUDEPATH += .. ../lpd-epilog ../ctrl-cut

include(common.pri)

CONFIG += qt gio
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

FORMS += CtrlCut.ui LaserDialog.ui SimulatorDialog.ui

RESOURCES += ctrl-cut.qrc

HEADERS += ./MainWindow.h \
           ./event/CtrlCutEvent.h \
           ./CtrlCutScene.h \
           ./NavigationView.h \
           ./Commands.h \
           ./GroupItem.h \
           ./EventFilter.h \
           ./LpdClient.h \
           ./CtrlCutView.h \
           ./LaserDialog.h \
           ./StreamUtils.h \
           ./settings/SettingsTableModel.h \
	   ./settings/CutSettingsTableModel.h \
	   ./settings/EngraveSettingsTableModel.h \
	   ./settings/DocumentSettingsTableModel.h \
           ./helpers/Qt.h \
           ./helpers/PathItem.h \
           ./helpers/CutItem.h \
           ./helpers/EngraveCanvas.h \
           ./helpers/EngraveItem.h \
           ./helpers/DocumentHolder.h \
           ./settings/CutSettingsTableModel.h \
           ./settings/EngraveSettingsTableModel.h \
           ./settings/DocumentSettingsTableModel.h \
           ./settings/SettingsTableModel.h
SOURCES += ./gui.cpp \
           ./event/CtrlCutEvent.cpp \
           ./Commands.cpp \
           ./LaserDialog.cpp \
           ./NavigationView.cpp \
           ./CtrlCutView.cpp \
           ./helpers/CutItem.cpp \
           ./helpers/PathItem.cpp \
           ./helpers/Qt.cpp \
           ./helpers/EngraveCanvas.cpp \
           ./helpers/EngraveItem.cpp \
           ./helpers/DocumentHolder.cpp \
           ./CtrlCutScene.cpp \
           ./LpdClient.cpp \
           ./MainWindow.cpp \
           ./StreamUtils.cpp
