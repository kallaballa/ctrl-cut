magick++ {
  message("Configuring Magick++...")

QMAKE_CXXFLAGS += `Magick++-config --cppflags --cxxflags --ldflags --libs`
QMAKE_LIBS += `Magick++-config --cppflags --cxxflags --ldflags --libs`
}
