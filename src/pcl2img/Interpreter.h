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
#include "Pcl.h"
#include "Signatures.h"
#include "Raster.h"
#include "Plot.h"

class Interpreter {
public:
  PclPlot plot;

  Interpreter(char* filename): plot(filename){
  };

  CImg<uint8_t>* renderRaster() {
    if(!this->plot.good()) {
      this->plot.invalidate("corrupt pcl header");
      return NULL;
    }
    dim width = 0;
    dim height = 0;

    if(plot.require(PCL_WIDTH) && plot.require(PCL_HEIGHT)) {
      width = plot.setting(PCL_WIDTH);
      height = plot.setting(PCL_HEIGHT);
    } else
      return NULL;

    PclInstr *xInstr = NULL, *yInstr = NULL, *pixlenInstr = NULL, *dataInstr = NULL, *yflipInstr = NULL;

    Run *run = new Run();
    PclPlotter* pclPlotter = new PclPlotter(width, height);
    RasterPlotter raster(pclPlotter);
    do {
      if ((yflipInstr = plot.readInstr()) && yflipInstr->matches(PCL_FLIPY)) {
        if(raster.currentRun != NULL)
          pclPlotter->doFlip(raster.currentRun->loc);
        else
          pclPlotter->doFlip(pclPlotter->origin);
      } else if (plot.currentInstr->matches(PCL_RASTER_START)) {
        pclPlotter->penDown();
      } else if (plot.currentInstr->matches(PCL_RASTER_END)){
        pclPlotter->penUp();
        continue;
      } else if (
          (yInstr = plot.currentInstr)->matches(PCL_Y,true)
          && (xInstr = plot.readInstr(PCL_X))
          && (pixlenInstr = plot.readInstr(PCL_PIXEL_LEN))
          && (dataInstr = plot.readInstr(PCL_RLE_DATA))
          ) {

        run->init(yInstr, xInstr, pixlenInstr, dataInstr);
        while (raster.decode(run) != NULL && !run->isFinished());
      }
    } while (this->plot.good());

    return pclPlotter->getCanvas();
  }

/*  BoundingBox& findBoundingBox() {
    if (!this->pclfile.good()) {
      this->pclfile.invalidate("corrupt pcl header");
      this->bbox.reset();
    } else {
      PclInstr *xInstr = NULL, *yInstr = NULL, *pixlenInstr = NULL, *rleInstr = NULL, *yflipInstr = NULL;

      do {
        if ((yflipInstr = pclfile.readInstr()) && yflipInstr->matches(PCL_FLIPY)) {
          yflip = !yflip;
          if(rleInstr && yflip)
            yflip_off = this->y;
          else
            yflip_off = 0;

          continue;
        } else if (pclfile.currentInstr->matches(PCL_RASTER_START) || pclfile.currentInstr->matches(PCL_RASTER_END)){
          continue;
        } else if (!(
            (yInstr = pclfile.currentInstr)->matches(PCL_Y,true)
            && (xInstr = pclfile.readInstr(PCL_X))
            && (pixlenInstr = pclfile.readInstr(PCL_PIXEL_LEN))
            && (rleInstr = pclfile.readInstr(PCL_RLE))
            )) {
          continue;
        }

        rleInstr->limit = rleInstr->value;
        this->x = xInstr->value;
        this->y = yInstr->value;
        this->decodedLen = abs(pixlenInstr->value);

        this->bbox.update(x,this->y + (this->y - this->yflip_off));
        this->bbox.update(x + decodedLen);
      } while (this->pclfile.good());
    }
    return this->bbox;
  }*/
};

#endif /* INTERPRETER_H_ */
