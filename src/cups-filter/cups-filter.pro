TARGET = cups-filter
#CONFIG += raster
CONFIG += gsapi
CONFIG += boost
CONFIG += cups
CONFIG += rsvg
CONFIG += cairo-ps
CONFIG += gio
CONFIG += boost_thread
CONFIG += libxml++
CONFIG += magick++
CONFIG += libctrl-cut

include(common.pri)

DEFINES += ETLOG DEBUG=4
HEADERS += ./CupsGetOpt.hpp \
           ./CupsOptions.hpp \

SOURCES += ./CupsGetOpt.cpp \
           ./CupsFilter.cpp \
           ./CupsOptions.cpp