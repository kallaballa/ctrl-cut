

#include "DitherSierra2.hpp"

void DitherSierra2::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& newpixel, const uint32_t colors) {
    Pixel<uint8_t> tmppix;

    this->img.readPixel(x, y, tmppix);
    if(tmppix.i <= 128)
      newpixel.i = 0;
    else
      newpixel.i = 255;
    this->img.writePixel(x,y, newpixel);

    const float quant_error = tmppix.i - newpixel.i;
    /*
               *   4   3
       1   2   3   2   1   (1/16)
     */
    if(quant_error != 0) {
      int8_t four =  round(quant_error * (4 / 16.0f));
      int8_t three = round(quant_error * (3 / 16.0f));
      int8_t two = round(quant_error * (2 / 16.0f));
      int8_t one = round(quant_error * (1 / 16.0f));

      carryOver(x + 1, y, four, tmppix);
      carryOver(x + 2, y, three, tmppix);

      carryOver(x - 2, y + 1, one, tmppix);
      carryOver(x - 1, y + 1, two, tmppix);
      carryOver(x,     y + 1, three, tmppix);
      carryOver(x + 1, y + 1, two, tmppix);
      carryOver(x + 2, y + 1, one, tmppix);
    }
}
