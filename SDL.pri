SDL {
  message("Configuring SDL...")
  DEFINES =+ PCLINT_USE_SDL
  macx: INCLUDEPATH += /opt/local/include/SDL
  else: INCLUDEPATH += /usr/include/SDL
  LIBS += -lSDL -lSDL_gfx -lSDLmain
  macx: LIBS += -framework Cocoa
}
