#ifndef CANVAS_H_
#define CANVAS_H_

#include <algorithm>
#ifdef PCLINT_USE_SDL
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>
#endif
#include "2D.hpp"
#include <string>
#include "CImg.hpp"

using std::string;
using cimg_library::CImg;

class Canvas {
public:
  Canvas(dim bedWidth, dim bedHeight, dim screenWidth = 0, dim screenHeight = 0, BoundingBox* clip = NULL);
  virtual ~Canvas() {};
  void drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b);
  void drawMove(coord x0, coord y0, coord x1, coord y1);
  void drawCut(coord x0, coord y0, coord x1, coord y1);
  void update();
  void dump(const string& filename, BoundingBox* clip = NULL);
private:
  class SDL_Surface *screen;
  dim bedWidth;
  dim bedHeight;

  dim screenWidth;
  dim screenHeight;

  BoundingBox* clip;
  CImg<uint8_t> offscreen;
  uint8_t intensity[1];
  double scale;

  void scaleCoordinate(coord& v) {
    v= (coord)((double) v) * scale;
  }
};

#endif /* CANVAS_H_ */
