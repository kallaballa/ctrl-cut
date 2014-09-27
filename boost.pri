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
  LIBS += -lboost_thread-mt
  LIBS += -lboost_iostreams-mt
  LIBS += -lboost_regex-mt
  LIBS += -lboost_filesystem-mt
  LIBS += -lboost_system-mt

  QMAKE_CXXFLAGS += -DBOOST_IOSTREAMS_USE_DEPRECATED
}

boost {
  message("Configuring Boost...")
  macx {
    INCLUDEPATH += /opt/local/include
  }
}

