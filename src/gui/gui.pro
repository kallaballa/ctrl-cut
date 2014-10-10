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
QT += network webkit widgets webkitwidgets

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

FORMS += CtrlCut.ui NewDialog.ui ImportDialog.ui PreviewDialog.ui SendDialog.ui

RESOURCES += ctrl-cut.qrc

HEADERS += ./MainWindow.hpp \
	   ./NewDialog.hpp \
     ./PreviewDialog.hpp \
	   ./ImportDialog.hpp \ 
           ./SendDialog.hpp \
           ./CtrlCutScene.hpp \
           ./NavigationView.hpp \
           ./Commands.hpp \
           ./GroupItem.hpp \
           ./EventFilter.hpp \
           ./LpdClient.hpp \
           ./CtrlCutView.hpp \
           ./StreamUtils.hpp \
	   ./ObjectPropertyWidget.hpp \
           ./helpers/Qt.hpp \
           ./helpers/GraphicsItems.hpp \
           ./helpers/PathItem.hpp \
           ./helpers/CutItem.hpp \
           ./helpers/EngraveItem.hpp \
           ./helpers/DocumentHolder.hpp
SOURCES += ./gui.cpp \
           ./Commands.cpp \
           ./NewDialog.cpp \
           ./PreviewDialog.cpp \
           ./SendDialog.cpp \
           ./ImportDialog.cpp \	   
           ./NavigationView.cpp \
           ./CtrlCutView.cpp \
           ./ObjectPropertyWidget.cpp \
           ./helpers/CutItem.cpp \
           ./helpers/PathItem.cpp \
           ./helpers/Qt.cpp \
           ./helpers/GraphicsItems.cpp \
           ./helpers/EngraveItem.cpp \
           ./helpers/DocumentHolder.cpp \
           ./CtrlCutScene.cpp \
           ./LpdClient.cpp \
           ./MainWindow.cpp \
           ./StreamUtils.cpp
