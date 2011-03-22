ImageMagick {
  message("Configuring ImageMagick...")
  DEFINES += cimg_use_magick
  INCLUDEPATH += /opt/local/include/ImageMagick
  LIBS += -L/opt/local/lib -lMagick++
}
