/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#include "DitherStucki.hpp"

void DitherStucki::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& newpixel, const uint32_t colors) {
    Pixel<uint8_t> tmppix;

    this->img.readPixel(x, y, tmppix);
    if(tmppix.i <= 127)
      newpixel.i = 0;
    else
      newpixel.i = 255;
    this->img.writePixel(x,y, newpixel);

    const float quant_error = tmppix.i - newpixel.i;

    /*
               *   8   4
       2   4   8   4   2
       1   2   4   2   1   (1/42)
     */
    if(quant_error != 0) {
      int8_t eight = round(quant_error * (8 / 42.0f));
      int8_t four =  round(quant_error * (4 / 42.0f));
      int8_t two = round(quant_error * (2 / 42.0f));
      int8_t one = round(quant_error * (1 / 42.0f));

      carryOver(x + 1, y, eight, tmppix);
      carryOver(x + 2, y, four, tmppix);

      carryOver(x - 2, y + 1, two, tmppix);
      carryOver(x - 1, y + 1, four, tmppix);
      carryOver(x,     y + 1, eight, tmppix);
      carryOver(x + 1, y + 1, four, tmppix);
      carryOver(x + 2, y + 1, two, tmppix);

      carryOver(x - 2, y + 2, one, tmppix);
      carryOver(x - 1, y + 2, two, tmppix);
      carryOver(x,     y + 2, four, tmppix);
      carryOver(x + 1, y + 2, two, tmppix);
      carryOver(x + 2, y + 2, one, tmppix);
    }
}
