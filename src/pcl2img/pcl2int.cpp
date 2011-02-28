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

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "CImg.h"
#include "Interpreter.h"

using std::string;
using std::cerr;
using std::cout;
using std::endl;

using namespace cimg_library;

void printUsage() {
  cerr << "usage" << endl;
}

int main(int argc, char *argv[]) {
  bool findBoundingBox = false;
  bool interactive = false;
  char* ifilename = NULL;
  char* ofilename = NULL;

  int c;
  opterr = 0;

  while ((c = getopt(argc, argv, "bi")) != -1)
    switch (c) {
    case 'b':
      findBoundingBox = true;
      break;
    case 'c':
      interactive = true;
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
  if(true) {
    Debugger::create(intr.plotter);
    Debugger::instance->setInteractive(true);
  }

  if(findBoundingBox) {
    //cout << "bounding box: " << intr.findBoundingBox() << endl;
  } else {
    intr.renderRaster();
//    intr.dumpCanvas(ofilename);
  }

  return 0;
}

