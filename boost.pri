boost_thread {
  CONFIG *= boost  # boost_thread Implies boost
  message("Configuring Boost thread...")
  macx {
    LIBS += -L/opt/local/lib
  }
  LIBS += -lboost_thread-mt 
}

boost {
  message("Configuring Boost...")

  LIBS += -lpthread
  macx {
    INCLUDEPATH += /opt/local/include
  }
}

