boost_thread {
  CONFIG *= boost  # boost_thread Implies boost
  message("Configuring Boost thread...")
  LIBS += -lboost_thread-mt
}

boost {
  message("Configuring Boost...")
  macx {
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib
  }
}

