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
#ifndef CANVAS_H_
#define CANVAS_H_

#include <algorithm>
#include "2D.h"
#include <string>
#include "CImg.h"

using std::string;
using cimg_library::CImg;

class Canvas {
public:
  Canvas(dim bedWidth, dim bedHeight, dim screenWidth = 0, dim screenHeight = 0, BoundingBox* clip = NULL);
  virtual ~Canvas() {};

  virtual void drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b) = 0;
  virtual void drawMove(coord x0, coord y0, coord x1, coord y1) = 0;
  virtual void drawCut(coord x0, coord y0, coord x1, coord y1) = 0;
  virtual void update() = 0;
  virtual void dumpVectorImage(const string& filename, BoundingBox* clip = NULL) = 0;
  virtual void dumpRasterImage(const string& filename, BoundingBox* clip = NULL) = 0;

  dim bedWidth;
  dim bedHeight;

  dim screenWidth;
  dim screenHeight;

  BoundingBox* clip;
  uint8_t intensity[1];
  double scale;

private:
  void scaleCoordinate(coord& v) {
    v= (coord)((double) v) * scale;
  }
};

#endif /* CANVAS_H_ */
