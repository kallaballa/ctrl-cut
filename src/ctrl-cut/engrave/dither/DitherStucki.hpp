/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef DITHERSTUCKI_H_
#define DITHERSTUCKI_H_

#include "Dither.hpp"

class DitherStucki: public Dither {
public:
  DitherStucki(GrayscaleImage& img): Dither(img) {}
  virtual ~DitherStucki(){}
  void ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors = 1);

};

#endif /* DITHERSTUCKI_H_ */
