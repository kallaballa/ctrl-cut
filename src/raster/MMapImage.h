/*
 * LargeImageSrc.h
 *
 *  Created on: Dec 23, 2010
 *      Author: amir
 */

#ifndef MMAPIMAGE_H_
#define MMAPIMAGE_H_

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include "RTypes.h"

using namespace std;
using namespace boost::interprocess;

class MMapImage {
private:
	file_mapping* m_file;
	mapped_region m_region;
  void * addr;
  std::size_t size;
  size_t bytes_per_pixel;
  size_t x;
  size_t y;
  size_t w;
  size_t h;

	const unsigned char* point2pointer(Point2D p);

public:
	string filename;
  MMapImage(string filename, size_t width, size_t height, offset_t x = 0, offset_t y = 0, offset_t region_off=0);
	MMapImage(file_mapping* m_file, string filename, size_t width, size_t height, offset_t x = 0, offset_t y = 0);
	virtual ~MMapImage(){}

  size_t offsetX();
  size_t offsetY();

	unsigned char pixel(Point2D p);
	size_t width();
	size_t height();

	MMapImage* tile(offset_t x, offset_t y, size_t width, size_t height);
};

#endif /* MMAPIMAGE_H_ */
