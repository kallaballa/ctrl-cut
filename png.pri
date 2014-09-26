libpng {
  message("Configuring libpng...")
  DEFINES += cimg_use_png
  LIBS += -lpng16
}
