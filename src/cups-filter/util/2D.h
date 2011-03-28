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

class Point2D
{
public:
  Point2D(int x, int y) { v[0] = x; v[1] = y; }
  int &operator[](size_t idx) { return v[idx]; }
  const int operator[](size_t idx) const { return v[idx]; }
  bool operator==(const Point2D &other) const {
    return this->v[0] == other.v[0] && this->v[1] == other.v[1];
  }
  float distance(const Point2D &other) const {
    return sqrt((v[0] - other.v[0]) * (v[0] - other.v[0]) +
                (v[1] - other.v[1]) * (v[1] - other.v[1]));
  }

protected:
  int v[2];
};

template<class T> class Pixel {
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

    //  just fix white instead of proper rounding
    if(i > 254.97f)
      i = 255;
    else if(i > 254.0f)
      i = 254;

    this->i = i;
  }

  void setGray(const T &val) {
    this->i = val;
  }

  uint8_t pclValue(float power_scale){
    return (uint8_t) (255 - this->i) * power_scale;
  }
};

class Rectangle {
public:
  Point2D ul, lr;

  //  Rectangle() { makeEmpty(); }

  Rectangle(const Point2D &ul, const Point2D &lr) : ul(ul), lr(lr) { }

  Rectangle(int ul_x = std::numeric_limits<int>::max(), int ul_y = std::numeric_limits<int>::max(), int lr_x = 0, int lr_y = 0) : ul(ul_x, ul_y), lr(lr_x, lr_y) { }

  bool inside(const Point2D &m, int tolerance = 0) {
    return (m[0] < (lr[0] + tolerance) && m[0] > (ul[0] - tolerance) &&
            m[1] < (lr[1] + tolerance) && m[1] > (ul[1] - tolerance));
  }
};

class BBox: public Rectangle {
public:
  bool inside(const Point2D &p, uint16_t tol_x = 0, uint16_t tol_y = 0) {
    return (p[0] < (lr[0] + tol_x) && p[0] > (ul[0] - tol_x) &&
            p[1] < (lr[1] + tol_y) && p[1] > (ul[1] - tol_y));
  }
  
  bool isValid() {
    return this->ul[0] < this->lr[0] &&
      this->ul[1] < this->lr[1] &&
      this->ul[0] < numeric_limits<int>::max() &&
      this->ul[1] < numeric_limits<int>::max() &&
      this->lr[0] >= 0 &&
      this->lr[1] >= 0;
  }

  void invalidate() {
    this->ul[0] = std::numeric_limits<int>::max();
    this->ul[1] = std::numeric_limits<int>::max();
    this->lr[0] = -1;
    this->lr[1] = -1;
  }

  void adjustTo(int x, int y) {
    this->ul[0] = min(x, ul[0]);
    this->ul[1] = min(y, ul[1]);
    this->lr[0] = max(x, lr[0]);
    this->lr[1] = max(y, lr[1]);
  }

  void adjustTo(const Point2D &m) {
    this->ul[0] = min(m[0], ul[0]);
    this->ul[1] = min(m[1], ul[1]);
    this->lr[0] = max(m[0], lr[0]);
    this->lr[1] = max(m[1], lr[1]);
  }

  void adjustTo(Rectangle* r) {
    this->ul[0] = min(r->ul[0], ul[0]);
    this->ul[1] = min(r->ul[1], ul[1]);
    this->lr[0] = max(r->lr[0], lr[0]);
    this->lr[1] = max(r->lr[1], lr[1]);
  }

  int distanceToOrigin(){
    return sqrt(this->ul[0] * this->ul[0] + this->ul[1] * this->ul[1]);
  }

};

#endif /* TWOD_H_ */
