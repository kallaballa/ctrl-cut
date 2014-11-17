/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#include "Dither.hpp"
#include "DitherThreshold.hpp"
#include "DitherBayer.hpp"
#include "DitherFloydSteinberg.hpp"
#include "DitherJarvis.hpp"
#include "DitherBurke.hpp"
#include "DitherStucki.hpp"
#include "DitherSierra2.hpp"
#include "DitherSierra3.hpp"

Dither& Dither::create(GrayscaleImage& img, EngraveSettings::Dithering dithering){
  //FIXME memory leak
  switch (dithering) {
    case EngraveSettings::BAYER:
      return *(new DitherBayer(img));
      break;
    case EngraveSettings::FLOYD_STEINBERG:
      return *(new DitherFloydSteinberg(img));
      break;
    case EngraveSettings::JARVIS:
      return *(new DitherJarvis(img));
      break;
    case EngraveSettings::BURKE:
      return *(new DitherBurke(img));
      break;
    case EngraveSettings::STUCKI:
      return *(new DitherStucki(img));
      break;
    case EngraveSettings::SIERRA2:
      return *(new DitherSierra2(img));
      break;
    case EngraveSettings::SIERRA3:
      return *(new DitherSierra3(img));
      break;
    default:
      assert(false);
      break;
  }
}

void Dither::carryOver(const uint32_t x, const uint32_t y, const int8_t carryover, Pixel<uint8_t>& p) {
  if (x < this->img.width() && y < this->img.height()) {
    this->img.readPixel(x, y, p);
    add(p, carryover);
    this->img.writePixel(x, y, p);
  }
}

BitmapImage Dither::dither(Point translate) {
  Pixel<uint8_t> pix;
  uint8_t byteAlignOff = (8 - ((int)std::round(translate.x)) % 8) % 8;
  uint32_t scanlineBreak = (this->img.width() - byteAlignOff);
  uint8_t scanlineRem = scanlineBreak % 8;
  scanlineBreak -= scanlineRem;

  uint32_t width = this->img.width() + (byteAlignOff ? 8 - byteAlignOff: 0);
  // Bitmaps width must be divisible by 8, so we pad to align it
  if (width % 8 != 0) width += (8 - width % 8);

  size_t size = this->img.height() * width / 8;
  void *dataptr = malloc(size);
  assert(dataptr);
  BitmapImage result(width, this->img.height(), dataptr);
  uint8_t *data = (uint8_t *)result.data();

  uint8_t bitmap;
  for (unsigned int y = 0; y < this->img.height(); ++y) {
    if (byteAlignOff) {
      bitmap = 0;

      // Pad leftmost bits with 1's
      bitmap |= ((0x01 << (8 - byteAlignOff)) - 1) << byteAlignOff;

      for (unsigned int x = 0; x < byteAlignOff; x++) {
        this->ditherPixel(x, y, pix);

        if (pix.i > 0) {
          bitmap |= (0x80 >> (8 - byteAlignOff + x));
        }
      }
      *(data++) = bitmap;
    }

    for (unsigned int x = 0; x < scanlineBreak; x+=8) {
      bitmap = 0;
      for (uint8_t b = 0; b < 8; ++b) {
        this->ditherPixel(x + b + byteAlignOff, y, pix);
        if (pix.i > 0) {
          bitmap |= (0x80 >> b);
        }
      }

      *(data++) = bitmap;
    }

    if (scanlineRem) {
      bitmap = 0;

      for (unsigned int x = 0; x < scanlineRem; ++x) {
        this->ditherPixel(x + scanlineBreak + byteAlignOff, y, pix);
        if (pix.i > 0) {
          bitmap |= (0x80 >> x);
        }
      }

      // Pad rightmost bits with 1's
      bitmap |= (0x01 << (8 - scanlineRem)) - 1;

      *(data++) = bitmap;
    }
  }

  assert(data <= ((uint8_t *)result.data() + size));

  return result;
}

void Dither::reduce(Pixel<uint8_t>& pix, const uint8_t colors) {
  float scaled = (float)round( colors * (float)(pix.i / 255.0f) );
  pix.i = round((float)(scaled / colors) * 255);
}

//FIXME how to guarantee T is unsigned and carry signed?
void Dither::add(Pixel<uint8_t>& pix, const int8_t carry) const {
  uint8_t sum = pix.i + carry;

  //overflow?
  if(carry > 0 && sum < pix.i)
    sum = 255;
  else if(carry < 0 && sum > pix.i)
    sum = 0;

  pix.i = sum;
}
