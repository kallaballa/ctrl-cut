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

#ifndef MMAPMATRIX_H_
#define MMAPMATRIX_H_

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include "stdint.hpp"
#include <cctype>
#include <limits>
#include <sstream>

using namespace boost::interprocess;
using std::numeric_limits;
using std::string;
using std::stringstream;
using std::ostream;

#define PCL_2D_MAX std::numeric_limits<uint32_t>::max()

typedef uint32_t coord;
typedef uint32_t dim;

class Point {
public:
  coord x;
  coord y;

  Point(coord x, coord y): x(x), y(y) {}
  Point(): x(0), y(0) {}

  Point& operator=(Point &p) {
    if(this != &p) {
      this->x = p.x;
      this->y = p.y;
    }
    return *this;
  }

  Point& operator=(const Point &p) {
    if(this != &p) {
      this->x = p.x;
      this->y = p.y;
    }
    return *this;
  }

  bool operator==(const Point &p) const {
    if(this == &p) {
      return true;
    } else
      return (this->x == p.x && this->y == p.y);
  }

  bool operator!=(const Point &p) const {
    return !(this->operator==(p));
  }

  bool operator==(Point &p) const {
    if(this == &p) {
      return true;
    } else
      return (this->x == p.x && this->y == p.y);
  }

  bool operator!=(Point &p) const {
    return !(this->operator==(p));
  }

  friend ostream& operator <<(ostream &os, Point &p) {
    os << "<" << p.x << "," << p.y << ">";
    return os;
  }
};

template<class T>
class Pixel {
public:
  T i;
  T r;
  T g;
  T b;

  static const float rf = 0.2989f;
  static const float gf = 0.5870f;
  static const float bf = 0.1140f;

  Pixel(){}

  /**
   * calculate intensity, invert it (black -> 255, white -> 0) and apply raster power scale
   */
  void setRGB(T* sampleOff) {
    this->r = *sampleOff;
    this->g = *(sampleOff + 1);
    this->b = *(sampleOff + 2);
  }

  T calculateIntensity() {
    float i = rf * r + gf * g + bf * b;

    //  just fix white instead of proper rounding
    if(i > 254.97f)
      i = 255;
    else if(i > 254.0f)
      i = 254;

    return (T)i;
  }

private:
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

  void update(Point& p) {
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
    return this->ul.x < this->lr.x &&
           this->ul.y < this->lr.y &&
           this->ul.x < numeric_limits<coord>::max() &&
           this->ul.y < numeric_limits<coord>::max();
  }

  friend ostream& operator <<(ostream &os, BoundingBox &bbox) {
    os << "<" << bbox.ul << "," << bbox.lr << ">";
    return os;
  }

  static BoundingBox* createFromGeometryString(string geom) {
    stringstream ss(geom);
    string strCoord;
    BoundingBox* bb = new BoundingBox();

    if(!getline(ss,strCoord, 'x'))
      return NULL;
    bb->ul.x = strtol(strCoord.c_str(), NULL, 10);
    if(!getline(ss,strCoord, 'x'))
      return NULL;
    bb->ul.y = strtol(strCoord.c_str(), NULL, 10);
    if(!getline(ss,strCoord, 'x'))
      return NULL;
    bb->lr.x = strtol(strCoord.c_str(), NULL, 10);
    if(!getline(ss,strCoord, 'x'))
      return NULL;
    bb->lr.y = strtol(strCoord.c_str(), NULL, 10);

    return bb;
  }
};


template<class T>
class MMapMatrix {
public:
	string filename;
	file_mapping* m_file;
	mapped_region m_region;
	void * addr;
	std::size_t size;
	size_t bytes_per_pixel;
	size_t x;
	size_t y;
	size_t w;
	size_t h;

	MMapMatrix(string filename, uint16_t width, uint16_t height,
			uint16_t x, uint16_t y, uint64_t region_off) {
		this->filename = filename;
		this->m_file = new file_mapping(filename.c_str(), read_only);
		this->bytes_per_pixel = sizeof(T) * 3;
		this->m_region = mapped_region(*this->m_file, read_only, region_off + (x
				* y * bytes_per_pixel), width * height * bytes_per_pixel);
		this->addr = m_region.get_address();
		this->size = m_region.get_size();
		this->x = x;
		this->y = y;
		this->w = width;
		this->h = height;
	}

	MMapMatrix(file_mapping* m_file, uint16_t width, uint16_t height,
			uint16_t x, uint16_t y) {
		this->filename = filename;
		this->m_file = m_file;
		this->bytes_per_pixel = sizeof(T) * 3;
		this->m_region = mapped_region(*this->m_file, read_only, x * y
				* bytes_per_pixel, width * height * bytes_per_pixel);
		this->addr = m_region.get_address();
		this->size = m_region.get_size();
		this->x = x;
		this->y = y;
		this->w = width;
		this->h = height;
	}

	size_t width() {
		return this->w;
	}

	size_t height() {
		return this->h;
	}

	size_t offsetX() {
		return this->x;
	}

	size_t offsetY() {
		return this->y;
	}

	void readPixel(const uint32_t x, const uint32_t y, Pixel<T>& pix) const {
	  T* sample = (static_cast<T*> (addr)) + ((y * w + x) * 3);
	  pix.setRGB(sample);
	}

  MMapMatrix<T>* tile(offset_t x, offset_t y, size_t width, size_t height) {
		return new MMapMatrix<T> (this->m_file, this->filename, width, height, x, y);
	}
};


typedef MMapMatrix<uint8_t> Image;
#endif /* MMAPMATRIX_H_ */

