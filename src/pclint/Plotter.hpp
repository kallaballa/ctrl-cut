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


#ifndef PLOTTER_H_
#define PLOTTER_H_
#include <map>
#include <iostream>
#include <stdint.h>
#include <cctype>
#include <sstream>
#include <string>
#include <limits>
#include "2D.hpp"
#include "PclIntConfig.hpp"
#include "HPGL.hpp"
#include "Statistic.hpp"
#include "Canvas.hpp"
#include "CLI.hpp"

using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::numeric_limits;

using namespace cimg_library;

class VectorPlotter {
private:
  BoundingBox *clip;
  bool down;
  Canvas *canvas;
  uint8_t intensity[1];
public:
  Point penPos;

  VectorPlotter(dim width, dim height, BoundingBox* clip = NULL) :
    clip(clip), down(false), penPos(0, 0) {
    if (clip != NULL) {
      width = clip->min(width, clip->lr.x - clip->ul.x);
      height = clip->min(height, clip->lr.y - clip->ul.y);
    }
    intensity[0] = 255;
    if(PclIntConfig::singleton()->screenSize != NULL)
      this->canvas = new Canvas(width, height, PclIntConfig::singleton()->screenSize->ul.x, PclIntConfig::singleton()->screenSize->ul.y);
    else
      this->canvas = new Canvas(width, height);
  }

  VectorPlotter(BoundingBox* clip = NULL) :
    clip(clip), down(false), canvas(NULL), penPos(0, 0) {
  }

  void penUp() {
    down = false;
    Statistic::singleton()->announcePenUp(SLOT_VECTOR);
  }

  void penDown() {
    down = true;
    Statistic::singleton()->announcePenDown(SLOT_VECTOR);
  }

  void move(coord x, coord y) {
    Point m(x, y);
    move(m);
  }

  void setIntensity(uint8_t intensity) {
    this->intensity[0] = intensity;
  }

  uint8_t getIntensity() {
    return this->intensity[0];
  }

  virtual void draw(const Point& from, const Point& to) {
    if(from == to) {
      Trace::singleton()->warn("zero length drawing operation?");
      return;
    }
    Point drawFrom = from;
    Point drawTo = to;

    coord clip_offX = 0;
    coord clip_offY = 0;

    //apply clipping and update bounding box
    if (this->clip) {
      drawTo = this->clip->shape(drawTo);
      drawFrom = this->clip->shape(drawFrom);
      clip_offX = clip->ul.x;
      clip_offY = clip->ul.y;
    }

    drawFrom.x -= clip_offX;
    drawFrom.y -= clip_offY;
    drawTo.x -= clip_offX;
    drawTo.y -= clip_offY;

    stringstream ss;
    ss << "\t\t" << drawFrom << " - " << drawTo << " i = " << (unsigned int)this->intensity[0];
    Trace::singleton()->debug(ss.str());

    canvas->drawCut(drawFrom.x, drawFrom.y, drawTo.x, drawTo.y);
  }

  void move(Point& to) {
    if (penPos != to) {
      if (down) {
        draw(penPos, to);
        Statistic::singleton()->announceWork(penPos, to, SLOT_VECTOR);
      } else {
        canvas->drawMove(penPos.x, penPos.y, to.x, to.y);
        Statistic::singleton()->announceMove(penPos, to, SLOT_VECTOR);
      }

      this->penPos = to;
      Trace::singleton()->logPlotterStat(penPos);
    }
  }

  virtual BoundingBox& getBoundingBox() {
    return Statistic::singleton()->getBoundingBox(SLOT_VECTOR);
  }

  virtual Canvas* getCanvas() {
    return canvas;
  }

  virtual void dumpCanvas(const string& filename) {
    if (PclIntConfig::singleton()->autocrop) {
      canvas->dump(filename,&getBoundingBox());
    } else {
      canvas->dump(filename);
    }
  }
};

class BitmapPlotter {
private:
  BoundingBox *clip;
  uint32_t width;
  uint32_t height;
  uint8_t *imgbuffer;

public:
  Point penPos;

  // width/height is given in bytes
  //
  BitmapPlotter(uint32_t width, uint32_t height, BoundingBox *clip = NULL) :
    clip(clip), width(width), height(height), penPos(0, 0) {
    if (clip != NULL) {
      this->width = clip->min(width, clip->lr.x - clip->ul.x);
      this->height = clip->min(height, clip->lr.y - clip->ul.y);
    }

    this->imgbuffer = new uint8_t[this->width * this->height];
    memset(this->imgbuffer, 0x00, this->width * this->height);
  }

  BitmapPlotter(BoundingBox* clip = NULL) :
    clip(clip), penPos(0, 0) {
    this->imgbuffer = NULL;
  }

  void move(coord x, coord y) {
    Point p(x, y);
    move(p);
  }

  void move(Point &to) {
    Statistic::singleton()->announceMove(penPos, to, SLOT_RASTER);
    this->penPos = to;
    // Trace::singleton()->logPlotterStat(penPos);
  }

  void writeBitmap(uint8_t bitmap) {
    this->fill(bitmap, 1);
  }

  void fill(uint8_t bitmap, int len) {
    int dir = (len < 0) ? -1 : 1;
    Point pos = this->penPos;
    Point from = pos;
    from.x *= 8;
    Point to = from;
    int delta = len * 8 - 1 * dir;
    if (int(from.x) + delta < 0) {
      Debugger::getInstance()->waitSteps();
      if (!Debugger::getInstance()->isInteractive()) assert(false);
    }
    to.x += delta;

    Statistic::singleton()->announcePenDown(SLOT_RASTER);
    Statistic::singleton()->announceWork(from, to, SLOT_RASTER);

    if (this->clip) {
      if (this->penPos.y < this->clip->ul.y || this->penPos.y > this->clip->lr.y) return;
      pos.x -= this->clip->ul.x;
      pos.y -= this->clip->ul.y;
    }
    for (int i=0;i<abs(len);i++) {
      if (this->clip) {
        if ((this->penPos.x + i) < this->clip->ul.x || (this->penPos.x + i) > this->clip->lr.x) return;
      }
     this->imgbuffer[pos.y * this->width + pos.x + i*dir] = bitmap;
    }
    Statistic::singleton()->announcePenUp(SLOT_RASTER);
  }

  virtual BoundingBox& getBoundingBox() {
    return Statistic::singleton()->getBoundingBox(SLOT_RASTER);
  }

  void dumpCanvas(const string& filename) {
    BoundingBox bbox = getBoundingBox();
    if(bbox.isValid()) {
      Point start(0,0);
      Point size(this->width * 8, this->height);
      if (PclIntConfig::singleton()->autocrop) {
        start.x = bbox.ul.x;
        start.y = bbox.ul.y;

        size.x = bbox.lr.x - bbox.ul.x + 1;
        size.y = bbox.lr.y - bbox.ul.y + 1;
      }

      CImg<uint8_t>* canvas = new CImg<uint8_t>(size.x, size.y, 1, 1, 255);

      uint8_t on = 0;
      for (uint32_t y=0;y<size.y;y++) {
        for (uint32_t x=0;x<(size.x/8);x++) {
          uint8_t bitmap = this->imgbuffer[(y + start.y)*this->width + (x + start.x/8)];
          for (int b=0;b<8;b++) {
            if ((bitmap & (0x80 >> b))) {
              canvas->draw_point(x*8 + b, y, &on);
            }
          }
        }
      }
      canvas->save(filename.c_str());
    }
  }
};

#endif /* PLOTTER_H_ */
