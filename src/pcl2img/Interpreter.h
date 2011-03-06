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

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include "Pcl.h"
#include "Signatures.h"
#include "Raster.h"
#include "Plot.h"
#include "CLI.h"

using std::string;
using std::stringstream;

class Interpreter {
public:
  PclPlotter* plotter;
  PclPlot plot;

  Interpreter(const char* filename,BoundingBox* crop=NULL): plotter(NULL), plot(filename){
    if(!this->plot.good()) {
      this->plot.invalidate("corrupt pcl header");
    }
    dim width = 0;
    dim height = 0;
    Point startPos(0,0);

    if(plot.require(PCL_WIDTH) && plot.require(PCL_HEIGHT)) {
      width = plot.setting(PCL_WIDTH);
      height = plot.setting(PCL_HEIGHT);
    } else
      this->plot.invalidate("can't find plot dimensions");

    if(plot.require(PCL_X) && plot.require(PCL_Y)) {
      startPos.x = plot.setting(PCL_X);
      startPos.y = plot.setting(PCL_Y);
    } else
      this->plot.invalidate("can't find start position");

    plotter = new PclPlotter(startPos, width, height, crop);
  };

  void renderRaster() {
    PclInstr *xInstr = NULL, *yInstr = NULL, *pixlenInstr = NULL, *dataInstr = NULL, *yflipInstr = NULL;
    Run *run = new Run();
    RasterPlotter raster(this->plotter);
    Point origin;
    do {
      if ((yflipInstr = nextInstr()) && yflipInstr->matches(PCL_FLIPY)) {
        if(raster.currentRun != NULL) {
          //Point flipAt = raster.currentRun->loc;

          //flipAt.x = raster.currentRun->decodedLen - flipAt.x;
          this->plotter->doFlip(raster.currentRun->loc);
        }
        else
          this->plotter->doFlip(origin);
      } else if (plot.currentInstr->matches(PCL_RASTER_START)) {
        continue;
      } else if (plot.currentInstr->matches(PCL_RASTER_END)){
        continue;
      } else if (
          (yInstr = plot.currentInstr)->matches(PCL_Y,true)
          && (xInstr = nextInstr(PCL_X))
          && (pixlenInstr = nextInstr(PCL_PIXEL_LEN))
          && (dataInstr = nextInstr(PCL_RLE_DATA))
          ) {
        run->init(yInstr, xInstr, pixlenInstr, dataInstr);
        run->nextRun();

        while(raster.decode(run)) {
          run->nextRun();
        }

        Debugger::instance->animate();
      }
    } while (this->plot.good());

    cerr << "Plot finished." << endl;
    Debugger::instance->setInteractive(true);
    Debugger::instance->announce(NULL);
  }

  PclInstr* nextInstr(const char* expected=NULL) {
    PclInstr* instr = plot.readInstr(expected);
    Debugger::instance->announce(instr);
    return instr;
  }
};

#endif /* INTERPRETER_H_ */
