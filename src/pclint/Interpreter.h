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

  PclInstr* nextPclInstr(PclPlot* pclPlot, const char* expected = NULL) {
    PclInstr* instr = pclPlot->readInstr(expected);
    Debugger::getInstance()->announce(instr);
    return instr;
  }

public:
  VectorPlotter* vectorPlotter;
  BitmapPlotter* bitmapPlotter;
  RtlPlot* rtlplot;

  Interpreter(RtlPlot* plot): width(0), height(0), bitmapPlotter(NULL), rtlplot(plot){
    //FIXME determine the primary plot parameters
/*
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
*/
    this->bitmapPlotter = new BitmapPlotter(21600/8, 14400, PclIntConfig::singleton()->clip);
    this->vectorPlotter = new VectorPlotter(21600,14400, PclIntConfig::singleton()->clip);
  };

  void renderPclPlot(PclPlot *pclPlot) {
    PclInstr *xInstr = NULL, *yInstr = NULL, *pixlenInstr = NULL, *dataInstr = NULL;
    Run *run = new Run();
    RasterDecoder raster(this->bitmapPlotter);
    Point origin;

    while (pclPlot->good()) {
        if (
            (yInstr = nextPclInstr(pclPlot,PCL_Y))
            && (xInstr = nextPclInstr(pclPlot,PCL_X))
            && (pixlenInstr = nextPclInstr(pclPlot,PCL_PIXEL_LEN))
            && (dataInstr = nextPclInstr(pclPlot,PCL_RLE_DATA))) {
          run->init(yInstr, xInstr, pixlenInstr, dataInstr);

          while (raster.decode(run))
            run->nextRun();

          Debugger::getInstance()->animate();
      }
    }
  }

  void renderHpglPlot(HpglPlot *hpglPlot) {
    HpglInstr* hpglInstr;

    while(hpglPlot->good() && (hpglInstr = hpglPlot->readInstr())) {
      if(PclIntConfig::singleton()->debugLevel >= LVL_DEBUG)
        cerr << *hpglInstr << endl;
      if(hpglInstr->matches("PU") || hpglInstr->matches("LTPU")) {
        vectorPlotter->penUp();
        vectorPlotter->move(hpglInstr->parameters[0], hpglInstr->parameters[1]);
      } else if(hpglInstr->matches("PD")) {
        vectorPlotter->penDown();
        vectorPlotter->move(hpglInstr->parameters[0], hpglInstr->parameters[1]);
      } else if(hpglInstr->matches("CONT")) {
        vectorPlotter->move(hpglInstr->parameters[0], hpglInstr->parameters[1]);
      }
    }
  }

  void render() {
    while(this->rtlplot->isValid()) {
      RtlContext activeContext = this->rtlplot->getActiveContext();

      if(activeContext == PCL_CONTEXT)
        renderPclPlot(this->rtlplot->requestPclPlot());
      else if(activeContext == HPGL_CONTEXT)
        renderHpglPlot(this->rtlplot->requestHPGLPlot());
      else
        break; //no context
    }
  }
};

#endif /* INTERPRETER_H_ */
