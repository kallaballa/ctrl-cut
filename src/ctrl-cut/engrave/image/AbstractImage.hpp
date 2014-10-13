/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef ABSTRACTIMAGE_H_
#define ABSTRACTIMAGE_H_

#include <stdint.h>
#include "util/2D.hpp"
#include "util/Logger.hpp"
#include <iostream>
class AbstractImage {
public:
  AbstractImage(uint32_t width, uint32_t height, void *addr = NULL) : 
    w(width), h(height), addr(addr) {
  }

  AbstractImage(const AbstractImage& other) :
    w(other.w), h(other.h), addr(other.addr), row_stride(other.row_stride) {
  }

  void setSize(uint32_t width, uint32_t height) {
    this->w = width;
    this->h = height;
  }
  uint32_t width() const { return this->w; }
  uint32_t height() const { return this->h; }

  void setRowstride(uint32_t stride) { this->row_stride = stride; }
  uint32_t rowstride() const { return this->row_stride; }

  virtual Rectangle autocrop(size_t bytewidth) const {
    if(!this->isAllocated())
      return Rectangle(0,0,0,0);

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

  void setData(void *addr) { this->addr = addr; }

  void *data() {
    if(!isAllocated())
      allocData();
    return this->addr;
  }

  bool isAllocated() const {
    return w > 0 && h > 0 && this->addr != NULL;
  }
protected:
  virtual void allocData() = 0;

  void performcopy(AbstractImage& dest, int bytewidth, int height, int xoffset, int yoffset) const {
    assert(isAllocated());
    const uint8_t *sourcedata = (uint8_t *) this->addr;
    uint8_t *destdata = (uint8_t *) dest.data();
    for (int j=0;j<height;j++) {
      for (int i=0;i<bytewidth;i++) {
        destdata[j*bytewidth + i] = sourcedata[(j + yoffset) * this->row_stride + i + xoffset];
      }
    }
    dest.setData(destdata);
  }

  uint32_t w;
  uint32_t h;

  void * addr;
  uint32_t row_stride;
};

#endif
