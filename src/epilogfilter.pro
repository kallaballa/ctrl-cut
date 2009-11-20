TARGET = EpilogFilter
macx {
  CONFIG -= app_bundle
  INCLUDEPATH += /opt/local/include 
}

LIBS += -lcups

SOURCES = \
          EpilogFilter.cpp \
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
          EpilogFilter.h \
          Joint.h \
          OnionSkin.h \
          CuttingOptimizer.h \
          LaserJob.h \
          Polygon.h \
          pjl.h \
          Driver.h \
          LineSegment.h \
          PrinterConnection.h
