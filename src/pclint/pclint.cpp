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
#include <fstream>
#include "CImg.h"
#include "Interpreter.h"
#include "2D.h"
#include "PclIntConfig.h"

using std::ifstream;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

using namespace cimg_library;

int main(int argc, char *argv[]) {
  PclIntConfig* config = PclIntConfig::singleton();
  config->parseCommandLine(argc,argv);
  ifstream *infile = new ifstream(config->ifilename, ios::in | ios::binary);
  RtlPlot* plot = new RtlPlot(infile);

  Interpreter intr(plot);
  if(config->interactive) {
    Debugger::create(intr.plotter);
    Debugger::getInstance()->setInteractive(true);
  }

  intr.render();
  if (config->ofilename != NULL) {
    CImg<uint8_t>* img = intr.plotter->getCanvas();
    if(img != NULL)
      img->save(config->ofilename);
    else
      cerr << "won't save an empty image" << endl;
  }

  cerr << "bounding box: " << *intr.plotter->getBoundingBox() << endl;
  return 0;
}
