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
#ifndef TWOD_H_
#define TWOD_H_

/** Number of bytes in the bitmap header. */
#define BITMAP_HEADER_NBYTES (54)

#include <string>
#include <iostream>
#include <stdint.h>
#include <limits>
#include <cmath>

using namespace std;

struct Point2D {
	Point2D(int x, int y) :
		x(x), y(y) {
	}
	int x, y;
};

template<class T>
class Pixel {
public:
  T i;
  static const float rf = 0.2989f;
  static const float gf = 0.5870f;
  static const float bf = 0.1140f;

	Pixel(){}

	/**
   * calculate intensity, invert it (black -> 255, white -> 0) and apply raster power scale
   */
	void setRGB(T* sampleOff) {
	  float i = rf * *sampleOff + gf * *(sampleOff + 1) + bf * *(sampleOff + 2);

    //  just fix black instead of proper rounding to save expensivess cpu time.
	  if(i > 254.0f)
	    i = 255;
	  this->i = i;
  }

	uint8_t pclValue(float power_scale){
	  return (uint8_t) (255 - this->i) * power_scale;
  }

private:
};

class Rectangle {
public:
	int ul_x, ul_y, lr_x, lr_y, width, height;

	Rectangle(int ul_x = std::numeric_limits<int>::max(), int ul_y = std::numeric_limits<int>::max(), int lr_x = 0, int lr_y = 0) {
		this->ul_x = ul_x;
		this->ul_y = ul_y;
		this->lr_x = lr_x;
		this->lr_y = lr_y;

		this->width = lr_x - ul_x;
		this->height = lr_y - ul_y;
	}

	bool inside(Point2D* m, int tolerance = 0) {
		return (m->x < (lr_x + tolerance) && m->x > (ul_x - tolerance) && m->y
				< (lr_y + tolerance) && m->y > (ul_y - tolerance));
	}
};

class BBox: public Rectangle {
 public:
  bool inside(uint16_t x, uint16_t y, uint16_t tol_x = 0, uint16_t tol_y = 0) {
    return (x < (lr_x + tol_x) && x > (ul_x - tol_x) && y < (lr_y + tol_y)
            && y > (ul_y - tol_y));
  }

  bool isValid() {
    return this->ul_x < this->lr_x &&
           this->ul_y < this->lr_y &&
           this->ul_x < numeric_limits<int>::max() &&
           this->ul_y < numeric_limits<int>::max() &&
           this->lr_x >= 0 &&
           this->lr_y >= 0;
  }

  void invalidate() {
    this->ul_x = std::numeric_limits<int>::max();
    this->ul_y = std::numeric_limits<int>::max();
    this->lr_x = -1;
    this->lr_y = -1;
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

  int distanceToOrigin(){
    return sqrt(this->ul_x * this->ul_x + this->ul_y * this->ul_y);
  }

};

#endif /* TWOD_H_ */
