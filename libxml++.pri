libxml++ {
  message("Configuring libxml++...")

QMAKE_CXXFLAGS +="`pkg-config --cflags libxml++-2.6`"
QMAKE_LIBS +="`pkg-config --libs libxml++-2.6`"
}
