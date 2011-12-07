/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "DitherJarvis.h"

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
