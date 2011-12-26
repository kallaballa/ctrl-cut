CTRLCUT_ROOT = $$PWD
message("Root dir:" $$CTRLCUT_ROOT)

isEmpty(VERSION) VERSION = $$system(date "+%Y.%m.%d")
DEFINES += CTRLCUT_VERSION=$$VERSION

CONFIG -= qt
OBJECTS_DIR = objects

macx {
  CONFIG -= app_bundle

  DEPLOYDIR = $$(MACOSX_DEPLOY_DIR)
  !isEmpty(DEPLOYDIR) {
    INCLUDEPATH += $$DEPLOYDIR/include
    LIBS += -L$$DEPLOYDIR/lib
  }
  # add CONFIG+=deploy to the qmake command-line to make a deployment build
  deploy {
    message("Building deployment version")
    CONFIG += x86 x86_64
  }
}

include(boost.pri)
include(ghostscript.pri)
include(X11.pri)
include(ImageMagick.pri)
include(png.pri)
include(cups.pri)
include(SDL.pri)
include(rsvg.pri)
include(cairo-ps.pri)
include(gio.pri)
include(libxml++.pri)
include(Magick++.pri)
include(libctrl-cut.pri)
