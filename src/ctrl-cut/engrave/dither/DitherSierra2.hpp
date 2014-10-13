/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef DITHERSIERRA2_H_
#define DITHERSIERRA2_H_

#include "Dither.hpp"

class DitherSierra2: public Dither {
public:
  DitherSierra2(GrayscaleImage& img): Dither(img) {}
  virtual ~DitherSierra2(){}
  void ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors = 1);
};

#endif /* DITHERSIERRA2_H_ */
