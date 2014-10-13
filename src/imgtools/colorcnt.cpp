/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include <stdint.h>
#include <iostream>
#include <set>
#include <string>
#include "CImg.hpp"

using namespace cimg_library;

using std::string;
using std::set;
using std::cout;
using std::cerr;
using std::endl;

colorcnt::colorcnt() {
  // TODO Auto-generated constructor stub
}

colorcnt::~colorcnt() {
  // TODO Auto-generated destructor stub
}

int mainssss(int argc, char *argv[]) {
  if(argc < 2 || argc > 4) {
    cerr << "Usage: colorcnt <image> <width> <height>" << endl;
    return 1;
  }
  const char *const fname = argv[1];

  CImg<uint8_t> img(fname);
  set<uint8_t> distinctColors;
  int w;
  int h;
  if(argc > 2)
    w = strtol(argv[2], NULL, 10);
  else
    w = img.width();

  if(argc > 3)
    h = strtol(argv[3], NULL, 10);
  else
    h = img.height();


  for(int y = 0; y < h; y++) {
    for(int x = 0; x < w; x++) {
      distinctColors.insert(img(x,y));
    }
  }

  cout << distinctColors.size() << endl;
  return 0;
}
