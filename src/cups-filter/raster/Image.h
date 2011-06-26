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

#ifndef IMAGE_H_
#define IMAGE_H_

#include "AbstractImage.h"
#include <fstream>
#include <string>
#include <iostream>
#include <stdint.h>
#include "util/Logger.h"
#include <iostream>
#include <assert.h>

class BitmapImage : public AbstractImage {
public:
  BitmapImage(uint32_t width, uint32_t height, uint8_t *buf = NULL) : AbstractImage(width, height, buf) {
    this->row_stride = width / 8; // Natural rowstride
  }

  /*!
    Returns the given subrectangle of this image as a new image.
  */
  AbstractImage *copy(const Rectangle &rect) {
    int w,h;
    rect.getSize(w, h);
    if (h == 0 || w == 0) return NULL;
    assert(w % 8 == 0);
    assert(rect.ul[0] % 8 == 0);
    BitmapImage *bitmapcopy = new BitmapImage(w, h);
    uint8_t *sourcedata = (uint8_t *)this->addr;
    uint8_t *destdata = (uint8_t *)malloc(bitmapcopy->row_stride * bitmapcopy->h);
    for (int j=0;j<h;j++) {
      for (int i=0;i<w/8;i++) {
        destdata[j*bitmapcopy->row_stride + i] = sourcedata[(j+rect.ul[1])*this->row_stride + i + rect.ul[0]/8];
      }
    }
    bitmapcopy->addr = destdata;
    bitmapcopy->xpos = rect.ul[0];
    bitmapcopy->ypos = rect.ul[1];
    return bitmapcopy;
  }

  Rectangle autocrop() {
    uint8_t *gsaddr = (uint8_t *)this->addr;

    int bytewidth = this->w / 8;
    int i,j;
    for (j=0;j<this->h;j++) {
      for (i=0;i<bytewidth;i++) {
        if (gsaddr[j * this->row_stride + i] != 0xff) break;
      }
      if (i != bytewidth) break;
    }
    int starty = j;
    for (j=this->h-1;j>=starty;j--) {
      for (i=0;i<bytewidth;i++) {
        if (gsaddr[j * this->row_stride + i] != 0xff) break;
      }
      if (i != bytewidth) break;
    }
    int endy = j;
    LOG_DEBUG(starty);
    LOG_DEBUG(endy);

    for (i=0;i<bytewidth;i++) {
      for (j=starty;j<=endy;j++) {
        if (gsaddr[j*this->row_stride + i] != 0xff) break;
      }
      if (j != endy+1) break;
    }
    int startx = i;
    for (i=bytewidth-1;i>=startx;i--) {
      for (j=starty;j<=endy;j++) {
        if (gsaddr[j*this->row_stride + i] != 0xff) break;
      }
      if (j != endy+1) break;
    }
    int endx = i;
    LOG_DEBUG(startx);
    LOG_DEBUG(endx);

    return Rectangle(startx*8, starty, (endx + 1) * 8, endy + 1);
  }

  bool saveAsPBM(const std::string &filename) {
    // Note: In the PBM format, 1 is black, 0 is white
    // What we get from ghostscript, and use internally, is opposite
    std::ofstream out(filename.c_str());
    out << "P4\n" << width() << " " << height() << "\n";
    uint8_t *invertedline = new uint8_t[width()/8];
    uint8_t *scanlineptr = (uint8_t *)addr;
    for (uint32_t j=0;j<height();j++) {
      for (uint32_t i=0;i<width()/8;i++) {
        invertedline[i] = ~(scanlineptr[i]);
      }
      out.write((const char *)invertedline, width()/8);
      scanlineptr += this->row_stride;
    }
    delete invertedline;
    return true;
  }
private:
};


template<class T>
class Image : public AbstractImage {
public:
  uint8_t bytes_per_pixel;
  uint8_t comp;

  Image(uint32_t width, uint32_t height, uint8_t components = 3, void *pixelbuffer = NULL) :
    AbstractImage(width, height, pixelbuffer), comp(components) {
    this->row_stride = width;
    this->bytes_per_pixel = sizeof(T) * components;
    LOG_DEBUG((int)this->bytes_per_pixel);
  }

  /*!
    Create sub tile using the same pixel buffer as the parent image
  */
  Image(Image *parent, uint32_t width, uint32_t height, uint32_t offsetx, uint32_t offsety) :
    AbstractImage(width, height) {
    translate(offsetx, offsety);
    this->row_stride = parent->row_stride;
    this->comp = parent->components();
    this->bytes_per_pixel = sizeof(T) * components;
    this->addr = (static_cast<T*>(parent->addr)) + (offsety * this->row_stride + offsetx) * this->comp;
  }

  virtual ~Image() {}
  
  AbstractImage *copy(const Rectangle &rect) { return NULL; }
  Rectangle autocrop() { }

  uint8_t components() const { return this->comp; }
  
  virtual void readPixel(const uint32_t x, const uint32_t y, Pixel<T>& pix) const {
    T* sample = (static_cast<T*> (addr)) + ((y * this->row_stride + x) * this->comp);
    if (this->comp == 1) pix.setGray(*sample);
    else pix.setRGB(sample);
  }

  virtual void writePixel(uint32_t x, uint32_t y, const Pixel<T>& pix) {
    T* sample = (static_cast<T*> (addr)) + ((y * this->row_stride + x) * this->comp);
    for (uint8_t i=0;i<this->comp;i++) {
      *(sample + i) = pix.i;
    }
  }

  bool saveAsPGM(const std::string &filename) {
    // Note: In the PBM format, 1 is black, 0 is white
    // What we get from ghostscript, and use internally, is opposite
    std::ofstream out(filename.c_str());
    out << "P5\n" << width() << " " << height() << "\n";
    out << "255\n";
    uint8_t *invertedline = new uint8_t[width()];
    uint8_t *scanlineptr = (uint8_t *)addr;
    for (uint32_t j=0;j<height();j++) {
      for (uint32_t i=0;i<width();i++) {
        invertedline[i] = ~(scanlineptr[i]);
      }
      out.write((const char *)invertedline, width());
      scanlineptr += this->row_stride;
    }
    delete invertedline;
    return true;
  }
};

typedef Image<uint8_t> GrayscaleImage;

#endif /* IMAGE_H_ */
