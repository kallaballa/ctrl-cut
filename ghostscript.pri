gsapi {
  message("Configuring Ghostscript...")
  DEFINES += USE_GHOSTSCRIPT_API
  macx {
   isEmpty(DEPLOYDIR) {
     INCLUDEPATH += /opt/local/include
     LIBS += -L/opt/local/lib
   }
  }
  LIBS += -lgs
}
