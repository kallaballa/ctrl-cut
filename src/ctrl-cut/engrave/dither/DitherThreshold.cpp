/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#include "DitherThreshold.hpp"

void DitherThreshold::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors) {
  this->img.readPixel(x, y, pix);
  if(pix.i <= 128)
    pix.i = 0;
  else
    pix.i = 255;
}
