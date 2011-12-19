TARGET = pclint
CONFIG -= qt
CONFIG += boost_thread
CONFIG += X11
CONFIG += libpng14
#CONFIG += ImageMagick
!macx:CONFIG += SDL

include(common.pri)

macx {
  CONFIG -= app_bundle
}

LIBS+= -lpthread
OBJECTS_DIR = objects
SOURCES += \
pclint.cpp \
SDLCanvas.cpp \
Canvas.cpp
