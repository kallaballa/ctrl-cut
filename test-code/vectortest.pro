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

HEADERS = ../src/LaserPass.h \
          ../src/VectorPass.h \
          ../src/Joint.h \
          ../src/LineSegment.h \
          ../src/PolyLine.h \
          ../src/pjl.h \
          ../src/OnionSkin.h


