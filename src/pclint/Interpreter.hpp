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
#include "Pcl.hpp"
#include "Raster.hpp"
#include "Plot.hpp"
#include "CLI.hpp"
#include "PclIntConfig.hpp"
#include "Plotter.hpp"

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
    this->bitmapPlotter = new BitmapPlotter(plot->getWidth()/8, plot->getHeight(), PclIntConfig::singleton()->clip);
    this->vectorPlotter = new VectorPlotter(plot->getWidth(),plot->getHeight(), PclIntConfig::singleton()->clip);
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
      }
    }
  }

  void renderHpglPlot(HpglPlot *hpglPlot) {
    HpglInstr* hpglInstr;

    while (hpglPlot->good() && (hpglInstr = hpglPlot->readInstr())) {
      if (PclIntConfig::singleton()->debugLevel >= LVL_DEBUG || PclIntConfig::singleton()->interactive) {
        cerr << *hpglInstr << endl;
      }
      Debugger::getInstance()->announce(hpglInstr);
      if (hpglInstr->matches(HPGL_PENUP) || hpglInstr->matches(HPGL_LTPENUP)) {
        vectorPlotter->penUp();
        if (hpglInstr->parameters[0] != (int32_t)HPGL_UNSET && 
           hpglInstr->parameters[1] != (int32_t)HPGL_UNSET) {
          vectorPlotter->move(hpglInstr->parameters[0], hpglInstr->parameters[1]);
        }
      } else if (hpglInstr->matches(HPGL_PENDOWN)) {
        vectorPlotter->penDown();
        if (hpglInstr->parameters[0] != (int32_t)HPGL_UNSET && 
            hpglInstr->parameters[1] != (int32_t)HPGL_UNSET) {
          vectorPlotter->move(hpglInstr->parameters[0], hpglInstr->parameters[1]);
        }
      } else if (hpglInstr->matches(HPGL_MOVE)) {
        if (hpglInstr->parameters[0] == (int32_t)HPGL_UNSET || 
            hpglInstr->parameters[1] == (int32_t)HPGL_UNSET) {
          Trace::singleton()->warn("MOVE without parameters");
        }
        vectorPlotter->move(hpglInstr->parameters[0], hpglInstr->parameters[1]);
      } else {
        stringstream ss;
        ss << "Unkown HPGL instruction: " << *hpglInstr;
        Trace::singleton()->warn(ss.str());
      }
    }
  }

  void render() {
    while (this->rtlplot->isValid()) {
      RtlContext activeContext = this->rtlplot->getActiveContext();

      if (activeContext == PCL_CONTEXT) {
        renderPclPlot(this->rtlplot->requestPclPlot());
      }
      else if (activeContext == HPGL_CONTEXT) {
        renderHpglPlot(this->rtlplot->requestHPGLPlot());
      }
      else {
        break; //no context
      }
    }
  }
};

#endif /* INTERPRETER_H_ */
