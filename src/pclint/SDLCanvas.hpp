#ifndef SDLCANVAS_H_
#define SDLCANVAS_H_

#include <algorithm>
#ifdef PCLINT_USE_SDL
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_image.h>
#endif
#include "Canvas.hpp"

using std::string;
using cimg_library::CImg;

class SDLCanvas : public Canvas {
public:
  SDLCanvas(dim bedWidth, dim bedHeight, dim screenWidth = 0, dim screenHeight = 0, BoundingBox* clip = NULL);
  virtual ~SDLCanvas() {
#ifdef PCLINT_USE_SDL
    SDL_FreeSurface(screen);
#endif
  };
  void drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b);
  void drawMove(coord x0, coord y0, coord x1, coord y1);
  void drawCut(coord x0, coord y0, coord x1, coord y1);
  void update();
  void dumpVectorImage(const string& filename, BoundingBox* clip = NULL);
  void dumpRasterImage(const string& filename, BoundingBox* clip = NULL);
private:
  class SDL_Surface *screen;
  dim bedWidth;
  dim bedHeight;

  dim screenWidth;
  dim screenHeight;

  BoundingBox* clip;
  CImg<uint8_t> voffscreen;
  CImg<uint8_t> roffscreen;
  uint8_t intensity[1];
  double scale;

  void scaleCoordinate(coord& v) {
    v= (coord)((double) v) * scale;
  }
};

#endif /* SDLCANVAS_H_ */
