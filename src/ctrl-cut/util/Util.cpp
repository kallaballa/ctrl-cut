/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "Util.hpp"
#include <boost/filesystem.hpp>
#include <csignal>
#include <set>
#include <cstdlib>

std::set<std::string> temp_file_names;

void delete_temp_files() {
  std::cerr << "Deleting temp files." << std::endl;
  for(const std::string& tmpfile : temp_file_names) {
    boost::filesystem::path file(tmpfile);
    if(boost::filesystem::exists(file))
      boost::filesystem::remove(file);
  }
}

void delete_temp_files_handler(int param) {
  std::cerr << "Caught SIGINT!" << std::endl;
  delete_temp_files();
  exit(1);
}

void install_signal_handlers() {
  signal(SIGINT, delete_temp_files_handler);
  std::atexit(delete_temp_files);
}

int lower_case (int c) {
  return tolower(c);
}

std::string make_temp_filename(const std::string suffix) {
  std::string tmpnam = (boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path()).native() + suffix;
  temp_file_names.insert(tmpnam);
  return tmpnam;
}

