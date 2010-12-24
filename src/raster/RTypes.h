#ifndef R_TYPES_H_
#define R_TYPES_H_

/** Number of bytes in the bitmap header. */
#define BITMAP_HEADER_NBYTES (54)

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include "RTypes.h"
#include <string>
#include <iostream>

using std::string;
using namespace boost::interprocess;

struct Point2D {
  int x, y;
};
class MMapImage {
private:
	file_mapping* m_file;
	mapped_region m_region;
  void * addr;
  std::size_t size;
  size_t bpp;
  size_t x;
  size_t y;
  size_t w;
  size_t h;

  offset_t point2offset(Point2D p);
	const unsigned char* point2addr(Point2D p);
public:
	string filename;

	MMapImage(string filename, size_t width, size_t height, size_t bpp = 8, offset_t x = 0, offset_t y = 0);
	MMapImage(file_mapping* m_file, string filename, size_t width, size_t height, size_t bpp = 8, offset_t x = 0, offset_t y = 0);
	virtual ~MMapImage(){}

	unsigned char pixel(Point2D p);
	size_t width();
	size_t height();

	MMapImage* tile(offset_t x, offset_t y, size_t width, size_t height);
};

typedef MMapImage Image;
typedef MMapImage Tile;
class Rectangle {
public:
  int ul_x, ul_y, lr_x, lr_y, width, height;
  
  Rectangle(int ul_x = 0, int ul_y = 0, int lr_x = 0, int lr_y = 0) {
    this->ul_x = ul_x;
    this->ul_y = ul_y;
    this->lr_x = lr_x;
    this->lr_y = lr_y;
    
    this->width = lr_x - ul_x;
    this->height = lr_y - ul_y;
  }
  
  bool inside(Point2D* m, int tolerance = 0) {
    return (m->x < (lr_x + tolerance) && m->x > (ul_x - tolerance) && m->y
            < (lr_y + tolerance) && m->y > (ul_y - tolerance));
  }
};

#endif /* R_TYPES_H_ */
