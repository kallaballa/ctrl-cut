boost_thread {
  CONFIG *= boost  # boost_thread Implies boost
  message("Configuring Boost thread...")
  macx {
    LIBS += -L/opt/local/lib
  }
  LIBS += -lboost_thread-mt
  LIBS += -lboost_iostreams-mt
  LIBS += -lboost_regex-mt
  LIBS += -lboost_filesystem
  LIBS += -lboost_system

  QMAKE_CXXFLAGS += -DBOOST_IOSTREAMS_USE_DEPRECATED
  INCLUDEPATH += /usr/local/inclue
  LIBS += -L/usr/local/lib64
}

boost {
  message("Configuring Boost...")
  macx {
    INCLUDEPATH += /opt/local/include
  }
}

