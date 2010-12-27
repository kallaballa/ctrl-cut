#ifndef R_TYPES_H_
#define R_TYPES_H_

/** Number of bytes in the bitmap header. */
#define BITMAP_HEADER_NBYTES (54)

#include "RTypes.h"
#include <string>
#include <iostream>

using namespace std;

struct Point2D {
  Point2D(int x, int y) : x(x), y(y) { }
  int x, y;
};

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
