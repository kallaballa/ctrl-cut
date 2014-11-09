boost {
  message("Configuring Boost...")

  macx {
    INCLUDEPATH += /opt/local/include
    LIBS += -L/opt/local/lib
  }
  else {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib64
  }

  QMAKE_CXXFLAGS += -DBOOST_IOSTREAMS_USE_DEPRECATED

  isEmpty(BOOST_LINK_FLAGS) {
    unix {
      BMT_TEST1 = /usr/lib64/libboost*thread-mt*
      BMT_TEST2 = /usr/lib/libboost*thread-mt*
      BMT_TEST3 = /usr/pkg/lib/libboost*thread-mt* # netbsd
      BMT_TEST4 = /usr/local/lib/libboost*thread-mt* # homebrew
      BMT_TEST5 = /opt/local/lib/libboost*thread-mt* # macports
      exists($$BMT_TEST1)|exists($$BMT_TEST2)|exists($$BMT_TEST3)|exists($$BMT_TEST4)|exists($$BMT_TEST5) {
        BOOST_LINK_FLAGS = -lboost_thread-mt -lboost_iostreams-mt -lboost_filesystem-mt -lboost_system-mt -lboost_regex-mt -lboost_program_options-mt
      }
    }
  }

  isEmpty(BOOST_LINK_FLAGS) {
    unix|macx {
      BOOST_LINK_FLAGS = -lboost_thread -lboost_iostreams -lboost_filesystem -lboost_system -lboost_regex -lboost_program_options
    }
  }

  LIBS += $$BOOST_LINK_FLAGS
}
