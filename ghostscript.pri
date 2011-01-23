gsapi {
  DEFINES += USE_GHOSTSCRIPT_API
  macx {
   INCLUDEPATH += /opt/local/include
   LIBS += -L/opt/local/lib
  }
  LIBS += -lgs
}
