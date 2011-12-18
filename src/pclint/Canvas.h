#ifndef CANVAS_H_
#define CANVAS_H_

#include <algorithm>
#include "2D.h"
#include <string>
#include "CImg.h"

using std::string;
using cimg_library::CImg;

class Canvas {
public:
  Canvas(dim bedWidth, dim bedHeight, dim screenWidth = 0, dim screenHeight = 0, BoundingBox* clip = NULL);
  virtual ~Canvas() {};

  virtual void drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b) = 0;
  virtual void drawMove(coord x0, coord y0, coord x1, coord y1) = 0;
  virtual void drawCut(coord x0, coord y0, coord x1, coord y1) = 0;
  virtual void update() = 0;
  virtual void dump(const string& filename, BoundingBox* clip = NULL) = 0;

  dim bedWidth;
  dim bedHeight;

  dim screenWidth;
  dim screenHeight;

  BoundingBox* clip;
  uint8_t intensity[1];
  double scale;

private:
  void scaleCoordinate(coord& v) {
    v= (coord)((double) v) * scale;
  }
};

#endif /* CANVAS_H_ */
