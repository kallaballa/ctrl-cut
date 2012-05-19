macx {
  CONFIG -= app_bundle
}

INCLUDEPATH += ../src /opt/local/include

SOURCES = vectortest.cpp \
          ../src/VectorPass.cpp \
          ../src/Joint.cpp \
          ../src/LineSegment.cpp \
          ../src/PolyLine.cpp \
          ../src/OnionSkin.cpp

HEADERS = ../src/LaserPass.hpp \
          ../src/VectorPass.hpp \
          ../src/Joint.hpp \
          ../src/LineSegment.hpp \
          ../src/PolyLine.hpp \
          ../src/pjl.hpp \
          ../src/OnionSkin.h


