
#ifndef DITHERBURKE_H_
#define DITHERBURKE_H_

#include "Dither.hpp"

class DitherBurke: public Dither {
public:
  DitherBurke(GrayscaleImage& img): Dither(img) {}
  virtual ~DitherBurke(){}
  void ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors = 1);
};

#endif /* DITHERBURKE_H_ */
