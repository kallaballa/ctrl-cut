boost_thread {
  CONFIG *= boost  # boost_thread Implies boost
  message("Configuring Boost thread...")
  macx {
    LIBS += -L/opt/local/lib
  }
  else {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib64
  }
  LIBS += -lboost_thread
  LIBS += -lboost_iostreams
  LIBS += -lboost_regex
  LIBS += -lboost_filesystem
  LIBS += -lboost_system

  QMAKE_CXXFLAGS += -DBOOST_IOSTREAMS_USE_DEPRECATED
}

boost {
  message("Configuring Boost...")
  macx {
    INCLUDEPATH += /opt/local/include
  }
}

