

#include "DitherJarvis.hpp"

void DitherJarvis::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& newpixel, const uint32_t colors) {
    Pixel<uint8_t> tmppix;

    this->img.readPixel(x, y, tmppix);
    if(tmppix.i <= 127)
      newpixel.i = 0;
    else
      newpixel.i = 255;
    this->img.writePixel(x,y, newpixel);

    const float quant_error = tmppix.i - newpixel.i;

    /*
               *   7   5
       3   5   7   5   3
       1   3   5   3   1   (1/48)
     */
    if(quant_error != 0) {
      int8_t seven = round(quant_error * (7 / 48.0f));
      int8_t five =  round(quant_error * (5 / 48.0f));
      int8_t three = round(quant_error * (3 / 48.0f));
      int8_t one =   round(quant_error * (1 / 48.0f));

      carryOver(x + 1, y, seven, tmppix);
      carryOver(x + 2, y, five, tmppix);

      carryOver(x - 2, y + 1, three, tmppix);
      carryOver(x - 1, y + 1, five, tmppix);
      carryOver(x,     y + 1, seven, tmppix);
      carryOver(x + 1, y + 1, five, tmppix);
      carryOver(x + 2, y + 1, three, tmppix);

      carryOver(x - 2, y + 2, one, tmppix);
      carryOver(x - 1, y + 2, three, tmppix);
      carryOver(x,     y + 2, five, tmppix);
      carryOver(x + 1, y + 2, three, tmppix);
      carryOver(x + 2, y + 2, one, tmppix);
    }
}
