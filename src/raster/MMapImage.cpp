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
#include "MMapImage.h"

MMapImage::MMapImage(string filename, size_t width, size_t height, offset_t x, offset_t y, offset_t region_off) {
  this->filename = filename;
  this->m_file = new file_mapping(filename.c_str(), read_only);
  this->bytes_per_pixel = sizeof(unsigned char);
  this->m_region = mapped_region(*this->m_file, read_only, region_off + (x * y * bytes_per_pixel), width * height * bytes_per_pixel);
  this->addr = m_region.get_address();
  this->size = m_region.get_size();
  this->x = x;
  this->y = y;
  this->w = width;
  this->h = height;
}

MMapImage::MMapImage(file_mapping* m_file, string filename, size_t width, size_t height, offset_t x, offset_t y) {
  this->filename = filename;
  this->m_file = m_file;
  this->m_region = mapped_region(*this->m_file, read_only, x * y * bytes_per_pixel, width * height * bytes_per_pixel);
  this->addr = m_region.get_address();
  this->size = m_region.get_size();
  this->bytes_per_pixel = sizeof(unsigned char);
  this->x = x;
  this->y = y;
  this->w = width;
  this->h = height;
}

size_t MMapImage::width() {
  return this->w;
}

size_t MMapImage::height() {
  return this->h;
}

size_t MMapImage::offsetX() {
  return this->x;
}

size_t MMapImage::offsetY() {
  return this->y;
}


const unsigned char* MMapImage::point2pointer(Point2D p) {
  return (static_cast<unsigned char*>(addr))+(p.y * w + p.x);
}

unsigned char MMapImage::pixel(Point2D p){
  return *point2pointer(p);
}

MMapImage* MMapImage::tile(offset_t x, offset_t y, size_t width, size_t height) {
  return new MMapImage(this->m_file, this->filename, width, height, x, y);
}

/*
  int main(int argc, char *argv[])
  {
  MMapImage* img = new MMapImage("/tmp/bla", 10, 10, 8, 0, 0);
  std::cerr << img->pixel({0,0}) << std::endl;
  std::cerr << img->pixel({1,0}) << std::endl;
  std::cerr << img->pixel({2,0}) << std::endl;
  std::cerr << img->pixel({3,0}) << std::endl;
  std::cerr << img->pixel({4,0}) << std::endl;
  std::cerr << img->pixel({5,0}) << std::endl;
  std::cerr << img->pixel({6,0}) << std::endl;
  std::cerr << img->pixel({7,0}) << std::endl;
  std::cerr << img->pixel({8,0}) << std::endl;
  std::cerr << img->pixel({9,0}) << std::endl;
  std::cerr << img->pixel({0,1}) << std::endl;
  std::cerr << img->pixel({1,1}) << std::endl;
  std::cerr << img->pixel({2,1}) << std::endl;
  std::cerr << img->pixel({3,1}) << std::endl;
  std::cerr << img->pixel({4,1}) << std::endl;
  std::cerr << img->pixel({5,1}) << std::endl;
  std::cerr << img->pixel({6,1}) << std::endl;
  std::cerr << img->pixel({7,1}) << std::endl;
  std::cerr << img->pixel({8,1}) << std::endl;
  std::cerr << img->pixel({9,1}) << std::endl;
  }
*/
