libpng {
  message("Configuring libpng...")
  DEFINES += cimg_use_png
  QMAKE_CXXFLAGS += `pkg-config --cflags libpng16`
  QMAKE_LIBS += `pkg-config --libs libpng16`
}
