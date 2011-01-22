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
#include "../util/Logger.h"
#include "RTypes.h"
#include "stdint.h"

using namespace std;
using namespace boost::interprocess;

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
	  LOG_DEBUG(region_off);
		this->filename = filename;
		this->m_file = new file_mapping(filename.c_str(), read_only);
		this->bytes_per_pixel = sizeof(T) * 3;
    LOG_DEBUG(bytes_per_pixel);
		this->m_region = mapped_region(*this->m_file, read_only, region_off + (x
				* y * bytes_per_pixel), width * height * bytes_per_pixel);
		this->addr = m_region.get_address();
		this->size = m_region.get_size();
    LOG_DEBUG(this->size);
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

	void readPixel(const int x, const int y, Pixel<T>& pix) const {
	  T* sample = (static_cast<T*> (addr)) + ((y * w + x) * 3);
	  pix.setRGB(sample);
	}

	MMapMatrix<T>* tile(offset_t x, offset_t y, size_t width, size_t height) {
		return new MMapMatrix<T> (this->m_file, this->filename, width, height, x, y);
	}
};

typedef MMapMatrix<uint8_t> Image;
#endif /* MMAPMATRIX_H_ */

