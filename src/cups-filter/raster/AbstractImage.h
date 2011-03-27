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

class AbstractImage {
public:
  AbstractImage(uint32_t width, uint32_t height, void *addr = NULL) : 
    w(width), h(height), addr(addr), xpos(0), ypos(0) {
  }
  virtual ~AbstractImage() {}

  uint32_t width() const { return this->w; }
  uint32_t height() const { return this->h; }
  uint32_t xPos() const { return this->xpos; }
  uint32_t yPos() const { return this->ypos; }

  void setRowstride(uint32_t stride) { this->row_stride = stride; }
  uint32_t rowstride() const { return this->row_stride; }


  void translate(uint32_t x, uint32_t y) {
    this->xpos += x;
    this->ypos += y;
  }
  
  void setData(void *addr) { this->addr = addr; }
  void *data() { return this->addr; }

protected:
  uint32_t w;
  uint32_t h;
  void * addr;

  uint32_t xpos;
  uint32_t ypos;
  uint32_t row_stride;

};

#endif
