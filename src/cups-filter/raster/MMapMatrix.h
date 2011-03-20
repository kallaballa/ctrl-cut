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

#include "AbstractImage.h"
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>

using namespace boost::interprocess;

template<class T>
class MMapMatrix : public AbstractImage<T> {
public:
  string filename;
  file_mapping* m_file;
  mapped_region m_region;
  std::size_t size;

  MMapMatrix(string filename, uint16_t width, uint16_t height,
             uint16_t x, uint16_t y, uint64_t region_off) :
    AbstractImage<T>(width, height, x, y, 3) {
    LOG_DEBUG(region_off);
    this->filename = filename;
    this->m_file = new file_mapping(filename.c_str(), read_write);
    this->m_region = mapped_region(*this->m_file, read_write, region_off + (x
                                                                            * y * this->bytes_per_pixel), width * height * this->bytes_per_pixel);
    this->addr = m_region.get_address();
    this->size = m_region.get_size();
    LOG_DEBUG(this->size);
  }

  MMapMatrix(file_mapping* m_file, uint16_t width, uint16_t height,
             uint16_t x, uint16_t y) :
    AbstractImage<T>(width, height, x, y, 3)
  {
    this->filename = filename;
    this->m_file = m_file;
    this->m_region = mapped_region(*this->m_file, read_write, x * y
                                   * this->bytes_per_pixel, width * height * this->bytes_per_pixel);
    this->addr = m_region.get_address();
    this->size = m_region.get_size();
  }

  MMapMatrix<T>* tile(offset_t x, offset_t y, size_t width, size_t height) {
    return new MMapMatrix<T> (this->m_file, this->filename, width, height, x, y);
  }
};


typedef MMapMatrix<uint8_t> MappedImage;
#endif /* MMAPMATRIX_H_ */

