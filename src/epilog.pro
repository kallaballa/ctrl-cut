TARGET = epilog
macx {
  CONFIG -= app_bundle
  INCLUDEPATH += /opt/local/include
}

SOURCES = \
          CupsParser.cpp \
          Joint.cpp \
          OnionSkin.cpp \
          RasterPass.cpp \
          CuttingOptimizer.cpp \
          LaserJob.cpp \
          Polygon.cpp \
          VectorPass.cpp \
          Driver.cpp \
          LineSegment.cpp \
          PrinterConnection.cpp


HEADERS = \
          CupsParser.h \
          Joint.h \
          OnionSkin.h \
          laser_config.h \
          CuttingOptimizer.h \
          LaserJob.h \
          Polygon.h \
          pjl.h \
          Driver.h \
          LineSegment.h \
          PrinterConnection.h
