libpng {
  message("Configuring libpng...")
  DEFINES += cimg_use_png
  PKGCONFIG += libpng16
}
