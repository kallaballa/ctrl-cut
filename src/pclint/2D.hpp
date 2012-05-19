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

#ifndef TWOD_H_
#define TWOD_H_

#include <boost/lexical_cast.hpp>
#include <cctype>
#include <limits>
#include <iostream>
#include <sstream>
#include <string>
#include <stdint.h>

using std::numeric_limits;
using std::string;
using std::stringstream;
using std::ostream;
using boost::lexical_cast;

#define PCL_2D_MAX std::numeric_limits<uint32_t>::max()

typedef uint32_t coord;
typedef uint32_t dim;

class Point {
public:
  coord x;
  coord y;

  Point(coord x, coord y): x(x), y(y) {}
  Point(): x(0), y(0) {}

  Point &operator=(const Point &p) {
    if (this != &p) {
      this->x = p.x;
      this->y = p.y;
    }
    return *this;
  }
  bool operator==(const Point &p) const {
    if (this == &p) {
      return true;
    } else {
      return (this->x == p.x && this->y == p.y);
    }
  }

  bool operator!=(const Point &p) const {
    return !(this->operator==(p));
  }

  friend ostream& operator <<(ostream &os, const Point &p) {
    os << "<" << p.x << "," << p.y << ">";
    return os;
  }
};

class BoundingBox {
public:
  Point ul;
  Point lr;
  BoundingBox(Point ul, Point lr): ul(ul), lr(lr) {}
  BoundingBox(): ul(PCL_2D_MAX, PCL_2D_MAX), lr(0,0){}

  void reset(){
    this->ul.x=PCL_2D_MAX;
    this->ul.y=PCL_2D_MAX;
    this->lr.x=0;
    this->lr.y=0;
  }

  void update(const Point& p) {
    update(p.x, p.y);
  }

  void update(coord x, coord y=PCL_2D_MAX) {
      this->ul.x = min(this->ul.x, x);
      this->lr.x = max(this->lr.x, x);

      if(y != PCL_2D_MAX) {
        this->ul.y = min(this->ul.y, y);
        this->lr.y = max(this->lr.y, y);
      }
  }

  coord min(const coord c1, const coord c2) const {
    return (c1<=c2?c1:c2);
  }

  coord max(const coord c1, const coord c2) const {
    return (c1>=c2?c1:c2);
  }

  Point& shape(Point &p) const {
    if(!inside(p)) {
      p.x = max(p.x, ul.x);
      p.x = min(p.x, lr.x);
      p.y = max(p.y, ul.y);
      p.y = min(p.y, lr.y);
    }
    return p;
  }

  bool inside(const Point &p) const {
    return (p.x <= lr.x && p.x >= ul.x && p.y <= lr.y
            && p.y >= ul.y);
  }

  bool isValid() const {
    return this->ul.x <= this->lr.x &&
           this->ul.y <= this->lr.y &&
           this->ul.x < numeric_limits<coord>::max() &&
           this->ul.y < numeric_limits<coord>::max();
  }

  friend ostream& operator <<(ostream &os, const BoundingBox &bbox) {
    os << "<" << bbox.ul << "," << bbox.lr << ">";
    return os;
  }

  BoundingBox operator+(const BoundingBox &bbox) const {
    BoundingBox enclosing = bbox;
    enclosing.update(this->ul);
    enclosing.update(this->lr);
    return enclosing;
  }

  BoundingBox& operator+=(const BoundingBox &bbox) {
    this->update(bbox.ul);
    this->update(bbox.lr);
    return *this;
  }

  static BoundingBox* createFromGeometryString(string geom) {
    stringstream ss(geom);
    string strCoord;
    BoundingBox* bb = new BoundingBox();

    if(!getline(ss,strCoord, 'x'))
      return NULL;
    bb->ul.x = strtol(strCoord.c_str(), NULL, 10);
    if(!getline(ss,strCoord, 'x'))
      return bb;
    bb->ul.y = strtol(strCoord.c_str(), NULL, 10);
    if(!getline(ss,strCoord, 'x'))
      return bb;
    bb->lr.x = strtol(strCoord.c_str(), NULL, 10);
    if(!getline(ss,strCoord, 'x'))
      return bb;
    bb->lr.y = strtol(strCoord.c_str(), NULL, 10);

    return bb;
  }
};
#endif /* TWOD_H_ */
