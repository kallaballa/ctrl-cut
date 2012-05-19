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
#include "CImg.hpp"
#include "Interpreter.hpp"
#include "2D.hpp"
#include "PclIntConfig.hpp"
#include "Plotter.hpp"
#include <stdlib.h>
#ifdef PCLINT_USE_SDL
#include <SDL.h>
#endif

using std::ofstream;
using std::ifstream;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

using namespace cimg_library;

int main(int argc, char *argv[]) {
  Trace* trace = Trace::singleton();
  PclIntConfig* config = PclIntConfig::singleton();
  config->parseCommandLine(argc, argv);
  ifstream *infile = new ifstream(config->ifilename, ios::in | ios::binary);
  RtlPlot* plot = new RtlPlot(infile);
  Statistic::init(plot->getWidth(), plot->getHeight(), plot->getResolution());

  Interpreter intr(plot);
  if (config->interactive) {
    Debugger::create(intr.vectorPlotter);
    Debugger::getInstance()->setInteractive(true);
  } else
    Debugger::create();

  intr.render();

  BoundingBox& vBox = intr.vectorPlotter->getBoundingBox();
  if (vBox.isValid()) {
    if (config->vectorFilename != NULL)
      intr.vectorPlotter->dumpCanvas(*new string(config->vectorFilename));
  } else {
    trace->warn("Vector image is empty.");
  }

  BoundingBox& bmpBox = intr.bitmapPlotter->getBoundingBox();
  if (bmpBox.isValid()) {
    if (config->rasterFilename != NULL)
      intr.bitmapPlotter->dumpCanvas(*new string(config->rasterFilename));
  } else {
    trace->warn("Bitmap image is empty.");
  }

  if (config->debugLevel >= LVL_INFO) {
    Statistic::singleton()->printSlot(cout, SLOT_VECTOR);
    Statistic::singleton()->printSlot(cout, SLOT_RASTER);
  }

  return 0;
}
