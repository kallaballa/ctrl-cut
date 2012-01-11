magick++ {
  message("Configuring Magick++...")

QMAKE_CXXFLAGS += `Magick++-config --cppflags --cxxflags`
QMAKE_LIBS += `Magick++-config --cppflags --cxxflags --ldflags --libs`
}
