gio {
  message("Configuring gio...")
  QMAKE_CXXFLAGS +="`pkg-config --cflags gio-unix-2.0`"
  QMAKE_LIBS +="`pkg-config --libs gio-unix-2.0`"
}
