TARGET = pclint
CONFIG -= qt

CONFIG += boost_thread
CONFIG += X11
#CONFIG += ImageMagick
CONFIG += libpng

include(common.pri)

macx {
  CONFIG -= app_bundle
}

QMAKE_LFLAGS = -ldl -lpthread

OBJECTS_DIR = objects

SOURCES += pclint.cpp
