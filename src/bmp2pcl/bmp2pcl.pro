TARGET = bmp2pcl
CONFIG -= qt
macx {
  CONFIG -= app_bundle
}
QMAKE_CFLAGS += -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing

OBJECTS_DIR = objects

SOURCES += bmp2pcl.cpp
