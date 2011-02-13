/*
 * Ctrl-Cut - A laser cutter CUPS driver
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

#ifndef GRID_H_
#define GRID_H_

#include "stdint.h"
#include <list>
#include <iostream>
#include <algorithm>
#include "boost/multi_array.hpp"

#include "RTypes.h"

using namespace std;

class BBox: public Rectangle {
 public:
  bool inside(uint16_t x, uint16_t y, uint16_t tol_x = 0, uint16_t tol_y = 0) {
    return (x < (lr_x + tol_x) && x > (ul_x - tol_x) && y < (lr_y + tol_y)
            && y > (ul_y - tol_y));
  }

  void adjustTo(int x, int y) {
    this->ul_x = min(x, ul_x);
    this->ul_y = min(y, ul_y);
    this->lr_x = max(x, lr_x);
    this->lr_y = max(y, lr_y);
  }

  void adjustTo(Point2D* m) {
    this->ul_x = min(m->x, ul_x);
    this->ul_y = min(m->y, ul_y);
    this->lr_x = max(m->x, lr_x);
    this->lr_y = max(m->y, lr_y);
  }

  void adjustTo(Rectangle* r) {
    this->ul_x = min(r->ul_x, ul_x);
    this->ul_y = min(r->ul_y, ul_y);
    this->lr_x = max(r->lr_x, lr_x);
    this->lr_y = max(r->lr_y, lr_y);
  }
};

typedef Rectangle PixelBox;

class DownSample {
	typedef boost::multi_array<PixelBox*, 2> PB_Image;
  PB_Image* image;
  list<PixelBox*> pixelBoxes;
  BBox boundingBox;
  uint16_t tolerance, pixel_width, pixel_height, res_x, res_y;

 public:
  DownSample(BBox* bbox, uint16_t res_x, uint16_t res_y,
     uint16_t pixel_width, uint16_t pixel_height, uint16_t tolerance);
    DownSample(Point2D* m_ul, uint16_t res_x, uint16_t res_y,
  		uint16_t pixel_width, uint16_t pixel_height, uint16_t tolerance);
  virtual ~DownSample(){}
  bool sample(BBox* bbox);
  bool sample(Point2D* m);
  BBox getBoundingBox();
  void resize(uint16_t width, uint16_t height);
  list<PixelBox*> getPixels();
};

#endif /* DOWNSAMPLE_H_ */

