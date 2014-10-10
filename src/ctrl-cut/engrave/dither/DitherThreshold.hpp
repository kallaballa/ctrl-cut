
#ifndef DITHERTHRESHOLD_H_
#define DITHERTHRESHOLD_H_

#include "Dither.hpp"

class DitherThreshold: public Dither {
public:
  DitherThreshold(GrayscaleImage& img): Dither(img) {}
  virtual ~DitherThreshold(){}
  void ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors = 32);

};

#endif /* DITHERTHRESHOLD_H_ */
