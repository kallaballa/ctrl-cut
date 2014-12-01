CTRLCUT_ROOT = $$PWD
message("Root dir:" $$CTRLCUT_ROOT)
isEmpty(VERSION) VERSION = $$system(git describe --abbrev=7 --dirty --always)
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

CONFIG(debug, debug|release) {
QMAKE_CXXFLAGS_DEBUG += -g3 -O0 -ggdb -Wc++0x-compat
message("DEBUG!")
DEFINES += CGAL_DISABLE_ROUNDING_MATH_CHECK=ON
} else {
QMAKE_CXXFLAGS += -msse2 -fprefetch-loop-arrays
DEFINES += QT_NO_DEBUG
DEFINES += QT_NO_DEBUG_OUTPUT
message("RELEASE!")
}

CONFIG += link_pkgconfig
QMAKE_CXXFLAGS += -std=c++11 -Wno-long-long -frounding-math


macx { # FIXME: Should really test for clang
  QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-deprecated-register
  QMAKE_CXXFLAGS += -stdlib=libc++
  QMAKE_LFLAGS += -stdlib=libc++
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
}

linux:QMAKE_CXXFLAGS += -pedantic-errors

include(CGAL.pri)
include(boost.pri)
include(ghostscript.pri)
include(X11.pri)
include(ImageMagick.pri)
include(rsvg.pri)
include(cairo-ps.pri)
include(png.pri)
include(gio.pri)
include(libxml++.pri)
include(Magick++.pri)
include(libctrl-cut.pri)

