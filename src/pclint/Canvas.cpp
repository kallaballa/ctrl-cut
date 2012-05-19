#include "Canvas.hpp"

Canvas::Canvas(dim bedWidth, dim bedHeight, dim screenWidth, dim screenHeight,
    BoundingBox* clip) :
  screen(NULL), bedWidth(bedWidth), bedHeight(bedHeight),
      screenWidth(screenWidth), screenHeight(screenHeight), clip(clip),
      offscreen(bedWidth, bedHeight, 1, 1, 255), scale(1) {
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

void Canvas::drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b) {
#ifdef PCLINT_USE_SDL
  if(screen != NULL) {
    scaleCoordinate(x0);
    scaleCoordinate(y0);

    pixelRGBA(screen, x0, y0, r, g, b, 128);
  }
#endif
}

void Canvas::drawMove(coord x0, coord y0, coord x1, coord y1) {
#ifdef PCLINT_USE_SDL
  if(screen != NULL) {
    scaleCoordinate(x0);
    scaleCoordinate(y0);
    scaleCoordinate(x1);
    scaleCoordinate(y1);
    lineRGBA(screen, x0, y0, x1, y1, 255, 255, 255, 128);
  }
#endif
}

void Canvas::drawCut(coord x0, coord y0, coord x1, coord y1) {
  offscreen.draw_line(x0, y0, x1, y1, this->intensity);
#ifdef PCLINT_USE_SDL
  if(screen != NULL) {
    scaleCoordinate(x0);
    scaleCoordinate(y0);
    scaleCoordinate(x1);
    scaleCoordinate(y1);
    lineRGBA(screen, x0, y0, x1, y1, 255, 0, 0, 128);
  }
#endif
}

void Canvas::update() {
#ifdef PCLINT_USE_SDL
  if(screen != NULL) {
    SDL_Flip(screen);
  }
#endif
}

void Canvas::dump(const string& filename, BoundingBox* crop) {
  if(crop != NULL)
    offscreen.crop(crop->ul.x, crop->ul.y, crop->lr.x, crop->lr.y, false).save(filename.c_str());
  else
    offscreen.save(filename.c_str());
}
