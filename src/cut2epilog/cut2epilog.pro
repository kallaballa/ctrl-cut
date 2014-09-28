TARGET = cut2epilog
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
CONFIG += cups
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += libpng
CONFIG += magick++
CONFIG += libctrl-cut

include(common.pri)

DEFINES += ETLOG DEBUG=4

SOURCES += ./Cut2Epilog.cpp
