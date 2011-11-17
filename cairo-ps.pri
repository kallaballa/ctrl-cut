cairo-ps {
  message("Configuring cairo-ps...")

QMAKE_CXXFLAGS +="`pkg-config --cflags cairo-ps`"
QMAKE_LIBS +="`pkg-config --libs cairo-ps`"
}
