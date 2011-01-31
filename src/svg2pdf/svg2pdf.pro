TARGET = svg2pdf
CONFIG -= qt
macx {
  CONFIG -= app_bundle
}
QMAKE_CFLAGS += `pkg-config --cflags librsvg-2.0 cairo-pdf` -Wall -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wnested-externs -fno-strict-aliasing
QMAKE_LIBS += `pkg-config --libs librsvg-2.0 cairo-pdf`

OBJECTS_DIR = objects

SOURCES += svg2pdf.c
