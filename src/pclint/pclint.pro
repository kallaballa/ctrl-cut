TARGET = pclint
CONFIG -= qt

CONFIG += boost_thread
CONFIG += X11
#CONFIG += ImageMagick
CONFIG += libpng
CONFIG += SDL

include(common.pri)

macx {
  CONFIG -= app_bundle
}

OBJECTS_DIR = objects

SOURCES += \
pclint.cpp \
Canvas.cpp
