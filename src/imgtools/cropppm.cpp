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
#include <fstream>
#include "MMapMatrix.h"
#include "PPMFile.h"

using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;

cropppm::cropppm() {}
cropppm::~cropppm() {}

int main(int argc, char *argv[]) {
  if(argc != 4) {
    cerr << "Usage: cropppm <geometry> <ppmin> <ppmout>" << endl;
    return 1;
  }
  string geom(argv[1]);
  string fin(argv[2]);
  ofstream out(argv[3]);

  BoundingBox* crop = BoundingBox::createFromGeometryString(geom);
  Image *img = loadppm(fin);
  dim oWidth = crop->lr.x - crop->ul.x;
  dim oHeight = crop->lr.y - crop->ul.y;
  out << "P6" << endl << oWidth << " " << oHeight << endl << "255" << endl;

  Pixel<uint8_t> pix;
  for (coord y = crop->ul.y; y < crop->lr.y; ++y) {
    for (coord x = crop->ul.x; x < crop->lr.x; ++x) {
      img->readPixel(x, y, pix);
      out << pix.r << pix.g << pix.b;
    }
  }

  return 0;
}
