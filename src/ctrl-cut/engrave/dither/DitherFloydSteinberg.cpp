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
