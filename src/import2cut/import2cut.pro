TARGET = import2cut
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += libxml++
CONFIG += magick++
CONFIG += libpng
CONFIG += libctrl-cut

include(common.pri)

DEFINES += ETLOG DEBUG=4

SOURCES += ./Import2Cut.cpp
