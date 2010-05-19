TARGET = epilog
macx {
  CONFIG -= app_bundle
  INCLUDEPATH += /opt/local/include
}

SOURCES = \
          src/CupsParser.cpp \
          src/Joint.cpp \
          src/OnionSkin.cpp \
          src/RasterPass.cpp \
          src/CuttingOptimizer.cpp \
          src/LaserJob.cpp \
          src/Polygon.cpp \
          src/VectorPass.cpp \
          src/Driver.cpp \
          src/LineSegment.cpp \
          src/PrinterConnection.cpp


HEADERS = \
          src/CupsParser.h \
          src/Joint.h \
          src/OnionSkin.h \
          src/laser_config.h \
          src/CuttingOptimizer.h \
          src/LaserJob.h \
          src/Polygon.h \
          src/pjl.h \
          src/Driver.h \
          src/LineSegment.h \
          src/PrinterConnection.h
