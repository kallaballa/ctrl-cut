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

#include "Pcl.h"
#include "Signatures.h"
#include "RLEDecoder.h"

class Interpreter {
public:
  PclPlot pclfile;

  Interpreter(char* filename): pclfile(filename){};

  CImg<uint8_t>* renderRaster() {
    if(!this->pclfile.good()) {
      this->pclfile.invalidate("corrupt pcl header");
      return NULL;
    }
    dim width = 0;
    dim height = 0;

    if(pclfile.require(PCL_WIDTH) && pclfile.require(PCL_HEIGHT)) {
      width = pclfile.setting(PCL_WIDTH);
      height = pclfile.setting(PCL_HEIGHT);
    } else
      return NULL;

    PclInstr *xInstr = NULL, *yInstr = NULL, *pixlenInstr = NULL, *dataInstr = NULL, *yflipInstr = NULL;

    Run *run = new Run();
    PclPlotter* plotter = new PclPlotter(width, height);
    RLEDecoder rledec(plotter);
    do {
      if ((yflipInstr = pclfile.readInstr()) && yflipInstr->matches(PCL_FLIPY)) {
        if(rledec.currentRun != NULL)
          plotter->doFlip(rledec.currentRun->loc);
        else
          plotter->doFlip(plotter->origin);
      } else if (pclfile.currentInstr->matches(PCL_RASTER_START)) {
        plotter->penDown();
      } else if (pclfile.currentInstr->matches(PCL_RASTER_END)){
        plotter->penUp();
        continue;
      } else if (
          (yInstr = pclfile.currentInstr)->matches(PCL_Y,true)
          && (xInstr = pclfile.readInstr(PCL_X))
          && (pixlenInstr = pclfile.readInstr(PCL_PIXEL_LEN))
          && (dataInstr = pclfile.readInstr(PCL_RLE_DATA))
          ) {

        run->init(yInstr, xInstr, pixlenInstr, dataInstr);
        while (rledec.decodeRLE(run) != NULL && !run->isFinished());
      }
    } while (this->pclfile.good());

    return plotter->getCanvas();
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
