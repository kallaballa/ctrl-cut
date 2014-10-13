/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef DITHERBAYER_H_
#define DITHERBAYER_H_

#include "Dither.hpp"

class DitherBayer: public Dither {
public:
  DitherBayer(GrayscaleImage& img): Dither(img) {}
  virtual ~DitherBayer(){}
  void ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors = 32);

};

#endif /* DITHERBAYER_H_ */
