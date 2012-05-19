macx {
  CONFIG -= app_bundle
}

INCLUDEPATH = ../src

SOURCES = ps_to_eps.cpp \
          ../src/eps_converter.cpp \
          ../src/laser_config.cpp

HEADERS = ../src/eps_converter.hpp \
          ../src/laser_config.h


