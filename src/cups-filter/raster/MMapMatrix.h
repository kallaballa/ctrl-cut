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

#include "Image.h"
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>

using namespace boost::interprocess;

template<class T>
class MMapMatrix : public Image<T> {
public:
  size_t offsetx;
  size_t offsety;
  string filename;
  file_mapping* m_file;
  mapped_region m_region;
  std::size_t size;

  MMapMatrix(string filename, uint32_t width, uint32_t height, uint8_t components = 3,
             uint32_t offsetx = 0, uint32_t offsety = 0, uint64_t region_off = 0) :
    Image<T>(width, height, components), offsetx(offsetx), offsety(offsety), filename(filename) {
    LOG_DEBUG(region_off);
    this->m_file = new file_mapping(filename.c_str(), read_write);
    this->m_region = mapped_region(*this->m_file, read_write, 
                                   region_off + (offsetx * offsety * this->bytes_per_pixel), 
                                   width * height * this->bytes_per_pixel);
    this->addr = m_region.get_address();
    this->size = m_region.get_size();
    LOG_DEBUG(this->size);
  }

  MMapMatrix(file_mapping* m_file, uint32_t width, uint32_t height, uint8_t components = 3,
             uint32_t offsetx = 0, uint32_t offsety = 0) :
    Image<T>(width, height, components), offsetx(offsetx), offsety(offsety), 
    filename(filename), m_file(m_file) {
    this->m_region = mapped_region(*this->m_file, read_write, 
                                   offsetx * offsety * this->bytes_per_pixel, 
                                   width * height * this->bytes_per_pixel);
    this->addr = m_region.get_address();
    this->size = m_region.get_size();
  }

  size_t offsetX() const { return this->offsetx; }
  
  size_t offsetY() const { return this->offsety; }
  
  MMapMatrix<T>* tile(offset_t x, offset_t y, size_t width, size_t height) {
    return new MMapMatrix<T> (this->m_file, this->filename, width, height, x, y);
  }
};


typedef MMapMatrix<uint8_t> MappedImage;
#endif /* MMAPMATRIX_H_ */

