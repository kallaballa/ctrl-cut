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

#ifndef ABSTRACTIMAGE_H_
#define ABSTRACTIMAGE_H_

#include <stdint.h>
#include "util/2D.hpp"
#include "util/Logger.hpp"

class AbstractImage {
public:
  AbstractImage(uint32_t width, uint32_t height, void *addr = NULL) : 
    w(width), h(height), addr(addr), shouldfree(false), xpos(0), ypos(0) {
  }
  virtual ~AbstractImage() {
    if (this->addr && shouldfree) free(this->addr);
  }

  virtual AbstractImage *copy(const Rectangle &rect) const = 0;

  void setSize(uint32_t width, uint32_t height) {
    this->w = width;
    this->h = height;
  }
  uint32_t width() const { return this->w; }
  uint32_t height() const { return this->h; }
  uint32_t xPos() const { return this->xpos; }
  uint32_t yPos() const { return this->ypos; }

  void setRowstride(uint32_t stride) { this->row_stride = stride; }
  uint32_t rowstride() const { return this->row_stride; }

  virtual Rectangle autocrop(size_t bytewidth) const {
    uint8_t *gsaddr = (uint8_t *)this->addr;

    unsigned int i,j;
    for (j=0;j<this->h;j++) {
      for (i=0;i<bytewidth;i++) {
        if (gsaddr[j * this->row_stride + i] != 0xff) break;
      }
      if (i != bytewidth) break;
    }
    unsigned int starty = j;
    for (j=this->h-1;j>=starty;j--) {
      for (i=0;i<bytewidth;i++) {
        if (gsaddr[j * this->row_stride + i] != 0xff) break;
      }
      if (i != bytewidth) break;
    }
    unsigned int endy = j;
    LOG_DEBUG(starty);
    LOG_DEBUG(endy);

    for (i=0;i<bytewidth;i++) {
      for (j=starty;j<=endy;j++) {
        if (gsaddr[j*this->row_stride + i] != 0xff) break;
      }
      if (j != endy+1) break;
    }
    unsigned int startx = i;
    for (i=bytewidth-1;i>=startx;i--) {
      for (j=starty;j<=endy;j++) {
        if (gsaddr[j*this->row_stride + i] != 0xff) break;
      }
      if (j != endy+1) break;
    }
    int endx = i;
    LOG_DEBUG(startx);
    LOG_DEBUG(endx);

    return Rectangle(startx, starty, (endx + 1), endy + 1);
  }

  virtual Rectangle autocrop() const = 0;

  void translate(uint32_t x, uint32_t y) {
    this->xpos += x;
    this->ypos += y;
  }
  void setTranslation(uint32_t x, uint32_t y) {
    this->xpos = x;
    this->ypos = y;
  }
  
  void setData(void *addr) { this->addr = addr; }
  void *data() { return this->addr; }
  virtual void *allocData() = 0;

protected:
  /*!
    Copy from this to dest. x coordinates are given in in bytes, not pixels
  */
  void performcopy(AbstractImage *dest,  
                   int bytewidth, int height, int xoffset, int yoffset) const {
    const uint8_t *sourcedata = (uint8_t *)this->addr;
    uint8_t *destdata = (uint8_t *)dest->allocData();
    for (int j=0;j<height;j++) {
      for (int i=0;i<bytewidth;i++) {
        destdata[j*bytewidth + i] = sourcedata[(j + yoffset) * this->row_stride + 
                                               i + xoffset];
      }
    }
    dest->addr = destdata;
    dest->xpos = xoffset;
    dest->ypos = yoffset;
  }

  uint32_t w;
  uint32_t h;
  void * addr;
  bool shouldfree;

  uint32_t xpos;
  uint32_t ypos;
  uint32_t row_stride;
};

#endif
