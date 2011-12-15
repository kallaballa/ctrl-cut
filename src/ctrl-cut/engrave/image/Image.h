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
  AbstractImage *copy(const Rectangle &rect) const {
    int w,h;
    rect.getSize(w, h);
    if (h == 0 || w == 0) return NULL;
    assert(w % 8 == 0);
    assert(rect.ul[0] % 8 == 0);
    BitmapImage *bitmapcopy = new BitmapImage(w, h);

    this->performcopy(bitmapcopy, w / 8, h, rect.ul[0] / 8, rect.ul[1]);
    bitmapcopy->xpos *= 8;
    return bitmapcopy;
  }

  Rectangle autocrop() const {
    Rectangle byterect = AbstractImage::autocrop(this->w / 8);
    return Rectangle(byterect.ul[0]*8, byterect.ul[1], byterect.lr[0] * 8, byterect.lr[1]);
  }

  void *allocData() {
    this->setData(malloc(this->h * this->w / 8));
    this->shouldfree = true;
    return this->data();
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
};


template<class T>
class Image : public AbstractImage {
public:
  uint8_t bytes_per_pixel;
  uint8_t comp;

  Image(uint32_t width, uint32_t height, uint8_t components, void *pixelbuffer = NULL) :
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
    this->addr = (static_cast<T*>(parent->addr)) + 
      (offsety * this->row_stride + offsetx) * this->comp;
  }

  virtual ~Image() {}
  
  /*!
    Returns the given subrectangle of this image as a new image.
  */
  AbstractImage *copy(const Rectangle &rect) const {
    int w,h;
    rect.getSize(w, h);
    if (h == 0 || w == 0) return NULL;

    Image<T> *imgcopy = new Image<T>(w, h, this->comp);

    this->performcopy(imgcopy, w * sizeof(T), h, rect.ul[0] * sizeof(T), rect.ul[1]);
    imgcopy->xpos /= sizeof(T);

    return imgcopy;
  }

  Rectangle autocrop() const {
    Rectangle byterect = AbstractImage::autocrop(this->w * sizeof(T));
    return Rectangle(byterect.ul[0] / sizeof(T), byterect.ul[1], 
                     byterect.lr[0] / sizeof(T), byterect.lr[1]);
  }

  void *allocData() {
    this->setData(malloc(this->h * this->w * sizeof(T)));
    this->shouldfree = true;
    return this->data();
  }

  uint8_t components() const { return this->comp; }
  
  virtual void readPixel(const uint32_t x, const uint32_t y, Pixel<T>& pix) const {
    assert(x < this->w && y < this->h);
    T* sample = (static_cast<T*> (addr)) + ((y * this->row_stride + x) * this->comp);
    if (this->comp == 1) pix.setGray(*sample);
    else pix.setRGB(sample);
  }

  virtual void writePixel(uint32_t x, uint32_t y, const Pixel<T>& pix) {
    assert(x < this->w && y < this->h);
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
