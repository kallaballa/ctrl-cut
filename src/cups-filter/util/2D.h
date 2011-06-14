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

#include <stdlib.h>
#include <stdint.h>
#include <limits>
#include <cmath>
#include <algorithm> // min(), max()

class Point2D
{
public:
  Point2D(int x, int y) { v[0] = x; v[1] = y; }
  int &operator[](size_t idx) { return v[idx]; }
  const int operator[](size_t idx) const { return v[idx]; }
  bool operator==(const Point2D &other) const {
    return this->v[0] == other.v[0] && this->v[1] == other.v[1];
  }
  bool operator!=(const Point2D &other) const {
    return !(*this == other);
  }
  float distance(const Point2D &other) const {
    return hypot(float(v[0] - other.v[0]), float(v[1] - other.v[1]));
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

  Rectangle(int ul_x = std::numeric_limits<int>::max(), 
            int ul_y = std::numeric_limits<int>::max(), 
            int lr_x = -std::numeric_limits<int>::max(),
            int lr_y = -std::numeric_limits<int>::max()) 
    : ul(ul_x, ul_y), lr(lr_x, lr_y) { }

  bool inside(const Point2D &p, uint16_t tol_x = 0, uint16_t tol_y = 0) const {
    return (p[0] < (lr[0] + tol_x) && p[0] > (ul[0] - tol_x) &&
            p[1] < (lr[1] + tol_y) && p[1] > (ul[1] - tol_y));
  }
  
  void getSize(int &width, int &height) {
    width = lr[0] - ul[0];
    height = lr[1] - ul[1];
  }

  int distanceToOrigin() const {
    return hypot(this->ul[0], this->ul[1]);
  }
};

class BBox: public Rectangle {
public:
  bool isValid() const {
    return (this->ul[0] < this->lr[0] && this->ul[1] < this->lr[1]);
  }

  void invalidate() {
    this->ul[0] = std::numeric_limits<int>::max();
    this->ul[1] = std::numeric_limits<int>::max();
    this->lr[0] = -std::numeric_limits<int>::max();
    this->lr[1] = -std::numeric_limits<int>::max();
  }

  void extendBy(const Point2D &m) {
    this->ul[0] = std::min(m[0], ul[0]);
    this->ul[1] = std::min(m[1], ul[1]);
    this->lr[0] = std::max(m[0], lr[0]);
    this->lr[1] = std::max(m[1], lr[1]);
  }

  void extendBy(const Rectangle &r) {
    this->ul[0] = std::min(r.ul[0], ul[0]);
    this->ul[1] = std::min(r.ul[1], ul[1]);
    this->lr[0] = std::max(r.lr[0], lr[0]);
    this->lr[1] = std::max(r.lr[1], lr[1]);
  }
};

#endif /* TWOD_H_ */
