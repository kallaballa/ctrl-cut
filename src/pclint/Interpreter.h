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
#include "PclIntConfig.h"

using std::string;
using std::stringstream;

class Interpreter {
private:
  dim width;
  dim height;

public:
  PclPlotter* plotter;
  PclPlot* plot;

  Interpreter(PclPlot* plot): width(0), height(0), plotter(NULL), plot(plot){
    if(!this->plot->good()) {
      this->plot->invalidate("corrupt pcl header");
    }
    Point startPos(0,0);

    if(this->plot->require(PCL_WIDTH) && this->plot->require(PCL_HEIGHT)) {
      this->width = this->plot->setting(PCL_WIDTH);
      this->height = this->plot->setting(PCL_HEIGHT);
    } else
      this->plot->invalidate("can't find plot dimensions");

    if(this->plot->require(PCL_X) && this->plot->require(PCL_Y)) {
      startPos.x = this->plot->setting(PCL_X);
      startPos.y = this->plot->setting(PCL_Y);
    } else
      this->plot->invalidate("can't find start position");

    this->plotter = new PclPlotter(this->width, this->height, PclIntConfig::singleton()->clip);
  };

  void render() {
    PclInstr *xInstr = NULL, *yInstr = NULL, *pixlenInstr = NULL, *dataInstr = NULL, *yflipInstr = NULL;
    Run *run = new Run();
    RasterPlotter raster(this->plotter);
    Point origin;
    do {
      if ((yflipInstr = nextInstr()) && yflipInstr->matches(PCL_FLIPY)) {
        continue;
      } else if (plot->currentInstr->matches(PCL_RASTER_START)) {
        continue;
      } else if (plot->currentInstr->matches(PCL_RASTER_END)){
        continue;
      } else if (
          (yInstr = plot->currentInstr)->matches(PCL_Y,true)
          && (xInstr = nextInstr(PCL_X))
          && (pixlenInstr = nextInstr(PCL_PIXEL_LEN))
          && (dataInstr = nextInstr(PCL_RLE_DATA))
          ) {
        run->init(yInstr, xInstr, pixlenInstr, dataInstr);

        while(raster.decode(run))
          run->nextRun();

        Debugger::getInstance()->animate();
      }
    } while (this->plot->good());

    cerr << "Plot finished." << endl;
  }

  PclInstr* nextInstr(const char* expected=NULL) {
    PclInstr* instr = plot->readInstr(expected);
    Debugger::getInstance()->announce(instr);
    return instr;
  }
};

#endif /* INTERPRETER_H_ */
