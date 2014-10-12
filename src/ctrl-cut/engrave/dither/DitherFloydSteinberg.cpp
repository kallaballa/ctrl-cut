

#include "DitherFloydSteinberg.hpp"

void DitherFloydSteinberg::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& newpixel, const uint32_t colors) {
    Pixel<uint8_t> tmppix;

    this->img.readPixel(x, y, tmppix);
    if(tmppix.i <= 128)
      newpixel.i = 0;
    else
      newpixel.i = 255;
    this->img.writePixel(x,y, newpixel);

    const float quant_error = tmppix.i - newpixel.i;
    /*
           *   7
       3   5   1     (1/16)
     */
    if(quant_error != 0) {
      int8_t seven = round(quant_error * (7 / 16.0f));
      int8_t five =  round(quant_error * (5 / 16.0f));
      int8_t three = round(quant_error * (3 / 16.0f));
      int8_t one =   round(quant_error * (1 / 16.0f));

      carryOver(x + 1, y, seven, tmppix);
      carryOver(x - 1, y + 1, three, tmppix);
      carryOver(x, y + 1, five, tmppix);
      carryOver(x + 1, y + 1, one, tmppix);
    }
}
