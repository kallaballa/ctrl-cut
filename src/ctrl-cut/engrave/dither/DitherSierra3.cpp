/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#include "DitherSierra3.hpp"

void DitherSierra3::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& newpixel, const uint32_t colors) {
    Pixel<uint8_t> tmppix;

    this->img.readPixel(x, y, tmppix);
    if(tmppix.i <= 127)
      newpixel.i = 0;
    else
      newpixel.i = 255;
    this->img.writePixel(x,y, newpixel);

    const float quant_error = tmppix.i - newpixel.i;

    /*
                 *   5   3
         2   4   5   4   2
             2   3   2       (1/32)
     */

    if(quant_error != 0) {
      int8_t five = round(quant_error * (5 / 32.0f));
      int8_t four =  round(quant_error * (4 / 32.0f));
      int8_t three =  round(quant_error * (3 / 32.0f));
      int8_t two = round(quant_error * (2 / 32.0f));

      carryOver(x + 1, y, five, tmppix);
      carryOver(x + 2, y, three, tmppix);

      carryOver(x - 2, y + 1, two, tmppix);
      carryOver(x - 1, y + 1, four, tmppix);
      carryOver(x,     y + 1, five, tmppix);
      carryOver(x + 1, y + 1, four, tmppix);
      carryOver(x + 2, y + 1, two, tmppix);


      carryOver(x - 1, y + 2, two, tmppix);
      carryOver(x,     y + 2, three, tmppix);
      carryOver(x + 1, y + 2, two, tmppix);
    }
}
