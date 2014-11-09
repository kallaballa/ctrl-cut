/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <boost/filesystem.hpp>

class Util {
public:
  static int lower_case (int c) {
    return tolower(c);
  }

  static string make_temp_filename() {
    return (boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path()).native();
  }
};

#endif /* UTIL_H_ */
