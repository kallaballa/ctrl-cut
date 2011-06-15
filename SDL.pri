SDL {
  message("Configuring SDL...")
  macx: INCLUDEPATH += /opt/local/include/SDL
  else: INCLUDEPATH += /usr/include/SDL
  LIBS += -lSDL -lSDL_gfx -lSDLmain
  macx: LIBS += -framework Cocoa
}
