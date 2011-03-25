/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdint.h>
#include <iostream>
#include <set>
#include <string>
#include "CImg.h"

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
