
#ifndef DITHERSIERRA_H_
#define DITHERSIERRA_H_

#include "Dither.hpp"

class DitherSierra3: public Dither {
public:
  DitherSierra3(GrayscaleImage& img): Dither(img) {}
  virtual ~DitherSierra3(){}
  void ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors = 1);

};

#endif /* DITHERSIERRA_H_ */
