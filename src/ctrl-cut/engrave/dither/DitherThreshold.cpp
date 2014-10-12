

#include "DitherThreshold.hpp"

void DitherThreshold::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors) {
  this->img.readPixel(x, y, pix);
  if(pix.i <= 128)
    pix.i = 0;
  else
    pix.i = 255;
}
