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

#ifndef RLEDECODER_H_
#define RLEDECODER_H_

#include "stdint.h"
#include "CImg.h"
#include "Pcl.h"
#include "2D.h"
#include "Plot.h"

class Run {
private:
  PclInstr* dataInstr;

public:
  Point loc;
  coord pos;

  uint8_t packedLen;
  uint8_t decodedLen;

  bool fill;
  bool reverse;
  bool valid;

  Run(): loc(0,0), valid(false) {}

  void init(PclInstr* yInstr, PclInstr* xInstr, PclInstr* pixlenInstr,PclInstr* dataInstr) {
    this->loc.y = yInstr->value;
    this->loc.x = xInstr->value;
    this->dataInstr = dataInstr;
    this->decodedLen = abs(pixlenInstr->value);
    this->reverse = pixlenInstr->value < 0;

    if(dataInstr->limit < 2) {
      Trace::singleton()->printBacklog(cerr, "short read: run length");
    } else {
      uint8_t rl = dataInstr->next();

      if(rl > 128) {
        this->fill = true;
        this->packedLen  = 257 - rl;
      } else if(rl < 128) {
        this->fill = false;
        this->packedLen  = rl + 1;
      } else {
        this->packedLen = 0;
      }
      this->valid = true;
    }
  }

  uint8_t nextIntensity() {
      if (!this->dataInstr->hasNext()) {
        Trace::singleton()->printBacklog(cerr, "short read: intensity");
        return 0;
      } else
      return 255 - this->dataInstr->next();
  }

  coord relativeX() {
      if(!this->reverse)
        return this->loc.x + this->decodedLen - (this->pos) - 1;
      else
        return this->loc.x + this->pos ;
  }

  coord relativeY() {
    return this->loc.y;
  }

  bool isFinished() {
    return this->pos >= this->decodedLen;
  }
};

class RasterPlotter {
private:
  PclPlotter* plotter;

public:
  Run* currentRun;

  RasterPlotter(PclPlotter* plotter): plotter(plotter), currentRun(NULL) {}

  Run* decode(Run *run) {
    this->currentRun = run;

    if(!run->valid || run->packedLen == 0)
      return NULL;

    if (run->fill) {
      plotter->intensity = run->nextIntensity();
      plotter->move(run->loc.x + run->packedLen, run->loc.y);
    } else {
      for (int i = 0; i < run->packedLen; ++i) {
        plotter->intensity = run->nextIntensity();
        plotter->move(run->loc.x - i, run->loc.y);
      }
    }

    return run;
  }
};
#endif /* RLEDECODER_H_ */
