rsvg {
  message("Configuring rsvg...")

QMAKE_CXXFLAGS += `pkg-config --cflags librsvg-2.0`
QMAKE_LIBS += `pkg-config --libs librsvg-2.0`
}
