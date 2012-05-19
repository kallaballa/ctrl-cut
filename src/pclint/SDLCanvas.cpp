#include "SDLCanvas.hpp"

SDLCanvas::SDLCanvas(dim bedWidth, dim bedHeight, dim screenWidth, dim screenHeight, BoundingBox* clip) :
Canvas(bedWidth, bedHeight, screenWidth, screenHeight, clip), voffscreen(bedWidth, bedHeight, 1, 1, 255), roffscreen(bedWidth, bedHeight, 1, 1, 255) {
#ifdef PCLINT_USE_SDL
  if (screenWidth > 0 && screenHeight > 0) {
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
      printf("Can't init SDL:  %s\n", SDL_GetError());
      exit(1);
    }
    atexit(SDL_Quit);
    screen = SDL_SetVideoMode(screenWidth, screenHeight, 16, SDL_SWSURFACE);
    if (screen == NULL) {
      printf("Can't set video mode: %s\n", SDL_GetError());
      exit(1);
    }
  }
#endif
  if (clip != NULL) {
    bedWidth = clip->min(bedWidth, clip->lr.x - clip->ul.x);
    bedWidth = clip->min(bedWidth, clip->lr.y - clip->ul.y);
  }
  intensity[0] = 0;

  double scale_x = (double) screenWidth / (double) bedWidth;
  double scale_y = (double) screenHeight / (double) bedHeight;

  scale = std::min(scale_x, scale_y);
}

void SDLCanvas::drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b) {
  roffscreen.draw_point(x0, y0, this->intensity);
#ifdef PCLINT_USE_SDL
  if(screen != NULL && screen->format != NULL) {
    scaleCoordinate(x0);
    scaleCoordinate(y0);

    pixelRGBA(screen, x0, y0, 0, 255, 0, 128);
  }
#endif
}

void SDLCanvas::drawMove(coord x0, coord y0, coord x1, coord y1) {
#ifdef PCLINT_USE_SDL
  if(screen != NULL && screen->format != NULL) {
    scaleCoordinate(x0);
    scaleCoordinate(y0);
    scaleCoordinate(x1);
    scaleCoordinate(y1);
    lineRGBA(screen, x0, y0, x1, y1, 255, 255, 255, 128);
  }
#endif
}

void SDLCanvas::drawCut(coord x0, coord y0, coord x1, coord y1) {
  voffscreen.draw_line(x0, y0, x1, y1, this->intensity);
#ifdef PCLINT_USE_SDL
  if(screen != NULL && screen->format != NULL) {
    scaleCoordinate(x0);
    scaleCoordinate(y0);
    scaleCoordinate(x1);
    scaleCoordinate(y1);
    lineRGBA(screen, x0, y0, x1, y1, 255, 0, 0, 128);
  }
#endif
}

void SDLCanvas::update() {
#ifdef PCLINT_USE_SDL
  if(screen != NULL && screen->format != NULL) {
    //SDL_Delay(100);
    SDL_Flip(screen);
  }
#endif
}

void SDLCanvas::dumpVectorImage(const string& filename, BoundingBox* clip) {
  if (!voffscreen.is_empty()) {
    if (clip != NULL)
      voffscreen.crop(clip->ul.x, clip->ul.y, clip->lr.x, clip->lr.y, false).save(
          filename.c_str());
    else
      voffscreen.save(filename.c_str());
  }
}

void SDLCanvas::dumpRasterImage(const string& filename, BoundingBox* clip) {
  if (!roffscreen.is_empty()) {
    if (clip != NULL)
      roffscreen.crop(clip->ul.x, clip->ul.y, clip->lr.x, clip->lr.y, false).save(
          filename.c_str());
    else
      roffscreen.save(filename.c_str());
  }
}
