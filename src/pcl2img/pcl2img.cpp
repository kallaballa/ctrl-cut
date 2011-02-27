/*
 * EpilogCUPS - A laser cutter CUPS driver
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

#include "Interpreter.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include <iostream>
#include "CImg.h"

using std::string;
using std::cerr;
using std::cout;
using std::endl;

using namespace cimg_library;

uint32_t* parseGeometry(char * optarg) {
  return NULL;
}

void printUsage() {
  cerr << "usage" << endl;
}

int main(int argc, char *argv[]) {
  bool findBoundingBox = false;
  uint32_t *crop = NULL;
  char* ifilename = NULL;
  char* ofilename = NULL;

  int c;
  opterr = 0;

  while ((c = getopt(argc, argv, "bc:")) != -1)
    switch (c) {
    case 'b':
      findBoundingBox = true;
      break;
    case 'c':
      crop = parseGeometry(optarg);
      break;
    case ':':
      printUsage();
      break;
    case '?':
      printUsage();
      break;
    }

  if (optind < argc) {
    ifilename = argv[optind];
  } else {
    printUsage();
    exit(1);
  }
  if (++optind < argc) {
    ofilename = argv[optind];
  } else {
    ofilename = "/dev/stdout";
  }

  Interpreter intr(ifilename);

  if(findBoundingBox) {
    //cout << "bounding box: " << intr.findBoundingBox() << endl;
  } else {
    CImg<uint8_t>* img = intr.renderRaster();
    if(!img) {
      cerr << "decoding failed!" << endl;
      return 1;
    }
    img->crop(400, 400, 1400, 7000);
    img->save(ofilename);
  }

  return 0;
}

