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

#ifndef RASTER_H_
#define RASTER_H_

#include <stdlib.h>
#include <stdint.h>
#include "CImg.hpp"
#include "Pcl.hpp"
#include "2D.hpp"
#include "Plot.hpp"
#include "Plotter.hpp"

class Run {
private:
  PclInstr* dataInstr;

public:
  Point loc; // X position given in bytes
  dim lineLen; // Pixel length in bytes
  coord linePos;

  bool fill;
  bool reverse;
  dim length;

  Run(): lineLen(0), linePos(0), length(0) {}

  Run* init(PclInstr* yInstr, PclInstr* xInstr, PclInstr* pixelLen,  PclInstr* dataInstr) {
    this->loc.y = yInstr->value;
    if(xInstr->value % 8 > 0)
      Trace::singleton()->warn("x is not a multiple of 8!");

    this->loc.x = xInstr->value / 8;
    this->lineLen = abs(pixelLen->value);
    this->linePos = 0;
    this->dataInstr = dataInstr;
    this->reverse = pixelLen->value < 0;
    return this->nextRun();
  }

  Run* nextRun() {
    if(dataInstr->limit < 2) {
      Trace::singleton()->printBacklog(cerr, "PCL RUN", "short read: run length");
      exit(2);
    } else {
      uint8_t rl = dataInstr->next();
      if(rl > 128) {
        this->fill = true;
        this->length  = 257 - rl;
      } else if(rl < 128) {
        this->fill = false;
        this->length  = rl + 1;
      } else {
        this->length = 0;
      }
      return this;
    }
  }

  uint8_t nextDataByte() {
    if (!this->dataInstr->hasNext()) {
      Trace::singleton()->printBacklog(cerr, "PCL RUN", "short read: intensity");
      exit(2);
    } else {
      return this->dataInstr->next();
    }
  }
};

class RasterDecoder {
private:
  BitmapPlotter *plotter;

public:
  Run *currentRun;

  RasterDecoder(BitmapPlotter *plotter) :
    plotter(plotter), currentRun(NULL) {
  }

  bool decode(Run *run) {
    this->currentRun = run;

    if (run == NULL || run->length == 0) {
      return NULL;
    }

    Point start = run->loc;

    if (run->reverse) {
      start.x += run->lineLen - run->linePos - 1;
    } else {
      start.x += run->linePos;
    }

    // string dirstring = run->reverse ? " <- " : " -> ";
    // if (run->reverse)
    //   cerr << "\t" << end << " <- " << start << endl;
    // else
    //   cerr << "\t" << start << " -> " << end << endl;

    this->plotter->move(start);

    int8_t dir = run->reverse ? -1 : 1;
    if (run->fill) {
      uint8_t bitmap = run->nextDataByte();
      plotter->fill(bitmap, run->length * dir);
    } else {
      for (dim i=0;i<run->length;i++) {
        plotter->move(start.x + i*dir, start.y);
        plotter->writeBitmap(run->nextDataByte());
      }
    }

    run->linePos += run->length;
    if (run->linePos < run->lineLen) {
      return true;
    }
    else {
      return false;
    }
  }
};
#endif /* RASTER_H_ */
