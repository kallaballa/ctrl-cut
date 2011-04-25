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
#include "Plotter.h"

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
  config->parseCommandLine(argc,argv);
  ifstream *infile = new ifstream(config->ifilename, ios::in | ios::binary);
  RtlPlot* plot = new RtlPlot(infile);
  Statistic::init(plot->getResolution());

  Interpreter intr(plot);
  if(config->interactive) {
    Debugger::create(intr.bitmapPlotter);
    Debugger::getInstance()->setInteractive(true);
  }

  intr.render();

  BoundingBox combinedBBox;

  if (intr.vectorPlotter->getBoundingBox().isValid())
    combinedBBox += intr.vectorPlotter->getBoundingBox();

  if (intr.bitmapPlotter->getBoundingBox().isValid())
    combinedBBox += intr.bitmapPlotter->getBoundingBox();

  if (config->vectorFilename || config->rasterFilename
      || config->combinedFilename) {
    CImg<uint8_t>* combinedImage = NULL;

    if (combinedBBox.isValid()) {
      combinedImage = new CImg<uint8_t> (combinedBBox.lr.x - combinedBBox.ul.x
          + 1, combinedBBox.lr.y - combinedBBox.ul.y + 1, 1, 1, 255);
    }

    if (intr.vectorPlotter->getBoundingBox().isValid()) {
      CImg<uint8_t>* vectorImage;

      if (combinedImage && !config->vectorFilename)
        vectorImage = intr.vectorPlotter->getCanvas(combinedImage);
      else if (config->combinedFilename) {
        vectorImage = intr.vectorPlotter->getCanvas();
        combinedImage->draw_image(*vectorImage);
      } else {
        vectorImage = intr.vectorPlotter->getCanvas();
      }

      if (config->vectorFilename != NULL)
        vectorImage->save(config->vectorFilename);
    } else {
      trace->warn("Vector image is empty.");
      ofstream combinedout(config->combinedFilename);
      combinedout << "";
    }

    if(intr.bitmapPlotter->getBoundingBox().isValid()) {
      CImg<uint8_t>* bitmapImage;

      if (combinedImage && !config->rasterFilename) {
        bitmapImage = intr.bitmapPlotter->getCanvas(combinedImage);
      } else if(config->combinedFilename) {
        bitmapImage = intr.bitmapPlotter->getCanvas();
        combinedImage->draw_image(*bitmapImage);
      } else {
        bitmapImage = intr.bitmapPlotter->getCanvas();
      }

      if (config->rasterFilename != NULL)
        bitmapImage->save(config->rasterFilename);
    } else {
      trace->warn("Bitmap image is empty.");
      ofstream combinedout(config->combinedFilename);
      combinedout << "";
    }

    if (config->combinedFilename) {
      if (combinedBBox.isValid()) {
        combinedImage->save(config->combinedFilename);
      } else {
        trace->warn("Blend image is empty.");
        ofstream combinedout(config->combinedFilename);
        combinedout << "";
      }
    }
  }

  if(config->debugLevel >= LVL_INFO) {
    Statistic::singleton()->printSlot(cout, SLOT_VECTOR);
    Statistic::singleton()->printSlot(cout, SLOT_RASTER);
  }
  return 0;
}
