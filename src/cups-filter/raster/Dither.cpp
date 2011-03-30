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


#include "Dither.h"
#include "DitherThreshold.h"
#include "DitherBayer.h"
#include "DitherFloydSteinberg.h"
#include "DitherJarvis.h"
#include "DitherBurke.h"
#include "DitherStucki.h"
#include "DitherSierra2.h"
#include "DitherSierra3.h"

Dither& Dither::create(GrayscaleImage& img, LaserConfig::RasterDithering dithering){
  switch (dithering) {
    case LaserConfig::DITHER_THRESHOLD:
      return *(new DitherThreshold(img));
      break;
    case LaserConfig::DITHER_BAYER:
      return *(new DitherBayer(img));
      break;
    case LaserConfig::DITHER_FLOYD_STEINBERG:
      return *(new DitherFloydSteinberg(img));
      break;
    case LaserConfig::DITHER_FLOYD_JARVIS:
      return *(new DitherJarvis(img));
      break;
    case LaserConfig::DITHER_FLOYD_BURKE:
      return *(new DitherBurke(img));
      break;
    case LaserConfig::DITHER_FLOYD_STUCKI:
      return *(new DitherStucki(img));
      break;
    case LaserConfig::DITHER_FLOYD_SIERRA2:
      return *(new DitherSierra2(img));
      break;
    case LaserConfig::DITHER_FLOYD_SIERRA3:
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

BitmapImage& Dither::dither() {
  Pixel<uint8_t> pix;
  uint8_t byteAlignOff = this->img.xPos() % 8;
  uint32_t scanlineBreak = (this->img.width() - byteAlignOff);
  uint8_t scanlineRem = scanlineBreak % 8;

  uint8_t bitmap;

  BitmapImage& result = *(new BitmapImage(img.width(), img.height()));
  size_t size = img.height() * img.width();
  void *dataptr = malloc(size);
  assert(dataptr);
  result.setData(dataptr);

  uint8_t* data = (uint8_t*)result.data();

  for (unsigned int y = 0; y < this->img.height(); ++y) {
    if(byteAlignOff) {
      bitmap = 0;
      for (unsigned int x = 0; x < byteAlignOff; x++) {
        this->ditherPixel(x,y, pix);

        if(pix.i > 0)
          bitmap |= (0x80 >> (8 - byteAlignOff));
      }
      *(data++) = bitmap;
    }

    for (unsigned int x = byteAlignOff; x < scanlineBreak; x+=8) {
      bitmap = 0;
      for (uint8_t b = 0; b < 8; ++b) {
        this->ditherPixel(x + b ,y, pix);
        if(pix.i > 0)
          bitmap |= (0x80 >> b);
      }

      *(data++) = bitmap;
    }

    if (scanlineRem) {
      bitmap = *data;

      for (unsigned int x = scanlineBreak; x < (scanlineBreak + scanlineRem); ++x) {
        this->ditherPixel(x, y, pix);
        if (pix.i > 127)
          bitmap |= (0x80 >> (x - scanlineBreak));
      }
      *(data++) = bitmap;
    }

    if(byteAlignOff) {
      bitmap = 0;
      for (unsigned int x = 0; x < byteAlignOff; x++) {
        this->ditherPixel(x,y, pix);

        if(pix.i > 127)
          bitmap |= (0x80 >> x);
      }
      *(data++) = bitmap;
    }
  }

  return result;
}

/*

void ditherBayer(const uint32_t x, const uint32_t y, Pixel<T>& pix, const uint32_t colors = 32) {
  this->img.readPixel(x, y, pix);
  pix.i = add(pix.i,bayer_matrix[(x/8) % 4][y % 4]);
  pix.i = reduce(pix.i, colors);
  this->img.writePixel(x, y, pix);
}




void ditherFloydSteinberg(const uint32_t x, const uint32_t y, Pixel<T>& newpixel, const uint32_t xrange = 8, const uint32_t colors = 32) {
  Pixel<T> oldpix;
  uint32_t fromX = x;
  uint32_t toX = x + xrange;
  uint32_t atY = y;

  averageXSequence(fromX, toX, atY, oldpix);
  newpixel.i = reduce(oldpix.i, colors);
  writeXSequence(fromX, toX, atY, newpixel);
  const float quant_error = oldpix.i - newpixel.i;

  if(quant_error != 0) {
    int8_t seven = (7 * quant_error) / 16;
    int8_t five = (5 * quant_error)  / 16;
    int8_t three = (3 * quant_error) / 16;
    int8_t one = (1 * quant_error)   / 16;

    fromX = x +  xrange;
    toX = fromX + xrange;

    carryOver(fromX, toX, atY, seven, oldpix);

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, three, oldpix);

    fromX = x;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, five, oldpix);

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, one, oldpix);
  }
}


void ditherJJN(const uint32_t x, const uint32_t y, Pixel<T>& newpixel, const uint32_t xrange = 8, const uint32_t colors = 32) {
  Pixel<T> oldpix;
  uint32_t fromX = x;
  uint32_t toX = x + xrange;
  uint32_t atY = y;

  averageXSequence(fromX, toX, atY, oldpix);
  newpixel.i = reduce(oldpix.i, colors);
  writeXSequence(fromX, toX, atY, newpixel);
  const float quant_error = oldpix.i - newpixel.i;

  if(quant_error > 0) {
    int8_t one = (1 * quant_error) / 48;
    int8_t three = (3 * quant_error) / 48;
    int8_t five = (5 * quant_error) / 48;
    int8_t seven = (7 * quant_error) / 48;

    fromX = x + xrange;
    toX = fromX + xrange;

    carryOver(fromX, toX, atY, seven, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;

    carryOver(fromX, toX, atY, five, oldpix);

    fromX = x - (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, three, oldpix);

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, five, oldpix);

    fromX = x;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, seven, oldpix);

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, five, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, three, oldpix);

    fromX = x - (xrange * 2);
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, one, oldpix);

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, three, oldpix);

    fromX = x;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, five, oldpix);

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, three, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, one, oldpix);
  }
}

void ditherStucki(const uint32_t x, const uint32_t y, Pixel<T>& newpixel, const uint32_t xrange = 8, const uint32_t colors = 16) {
  Pixel<T> oldpix;
  uint32_t fromX = x;
  uint32_t toX = x + xrange;
  uint32_t atY = y;

  averageXSequence(fromX, toX, atY, oldpix);
  newpixel.i = reduce(oldpix.i, colors);
  writeXSequence(fromX, toX, atY, newpixel);
  const float quant_error = oldpix.i - newpixel.i;

  if(quant_error > 0) {
    int8_t eight = (8 * quant_error) / 42;
    int8_t four = (4 * quant_error) / 42;
    int8_t two = (2 * quant_error) / 42;
    int8_t one = (1 * quant_error) / 42;

    fromX = x + xrange;
    toX = fromX + xrange;

    carryOver(fromX, toX, atY, eight, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;

    carryOver(fromX, toX, atY, four, oldpix);

    fromX = x - (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, two, oldpix);

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, four, oldpix);

    fromX = x;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, eight, oldpix);

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, four, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, two, oldpix);

    fromX = x - (xrange * 2);
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, one, oldpix);

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, two, oldpix);

    fromX = x;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, four, oldpix);

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, two, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, one, oldpix);
  }
}

void ditherBurke(const uint32_t x, const uint32_t y, Pixel<T>& newpixel, const uint32_t xrange = 8, const uint32_t colors = 16) {
  Pixel<T> oldpix;
  uint32_t fromX = x;
  uint32_t toX = x + xrange;
  uint32_t atY = y;

  averageXSequence(x, toX, y, oldpix);
  newpixel.i = (((uint8_t) (((float)colors) * (oldpix.i / 255.0f))) / ((float)colors)) * 255;
  writeXSequence(x, toX, y, newpixel);
  const float quant_error = oldpix.i - newpixel.i;

  if(quant_error > 0) {


    int8_t eight = (8 * quant_error) / 32;
    int8_t four = eight >> 1;
    int8_t two = four >> 1;

    fromX = x + xrange;
    toX = fromX + xrange;

    if (toX < w) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, eight);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }

    fromX = x + (xrange * 2);
    toX = fromX + xrange;

    if (toX < w) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, four);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }

    fromX = x - (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    if (x >= (xrange * 2) && y >= 1) {
      averageXSequence(fromX, toX, atY, oldpix);
      oldpix.i = add(oldpix.i, two);
      writeXSequence(fromX, toX, atY, oldpix);
    }

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 1;

    if (x >= xrange && y >= 1) {
      averageXSequence(fromX, toX, atY, oldpix);
      oldpix.i = add(oldpix.i, four);
      writeXSequence(fromX, toX, atY, oldpix);
    }

    fromX = x;
    toX = fromX + xrange;
    atY = y - 1;

    if (toX < w && y >= 1) {
      averageXSequence(x, toX, atY, oldpix);
      oldpix.i = add(oldpix.i, eight);
      writeXSequence(x, toX, atY, oldpix);
    }

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 1;

    if (toX < w && y >= 1) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, four);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }

    fromX = x + (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    if (toX < w && y >= 1) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, two);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }
  }
}

void ditherSierra3(const uint32_t x, const uint32_t y, Pixel<T>& newpixel, const uint32_t xrange = 8, const uint32_t colors = 32) {
  Pixel<T> oldpix;
  uint32_t fromX = x;
  uint32_t toX = x + xrange;
  uint32_t atY = y;

  averageXSequence(fromX, toX, atY, oldpix);
  newpixel.i = reduce(oldpix.i, colors);
  writeXSequence(fromX, toX, atY, newpixel);
  const float quant_error = oldpix.i - newpixel.i;

  if(quant_error > 0) {

    int8_t five = (5 * quant_error) / 32;
    int8_t four = (4 * quant_error) / 32;
    int8_t three = (3 * quant_error) / 32;
    int8_t two = (2 * quant_error) / 32;

    fromX = x + xrange;
    toX = fromX + xrange;

    carryOver(fromX, toX, atY, five, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;

    carryOver(fromX, toX, atY, three, oldpix);

    fromX = x - (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, two, oldpix);

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, four, oldpix);

    fromX = x;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, five, oldpix);

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, four, oldpix);

    fromX = x + (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    carryOver(fromX, toX, atY, two, oldpix);

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, two, oldpix);

    fromX = x;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, three, oldpix);

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 2;

    carryOver(fromX, toX, atY, two, oldpix);
  }
}

void ditherSierra2(const uint32_t x, const uint32_t y, Pixel<T>& newpixel, const uint32_t xrange = 8, const uint32_t colors = 16) {
  Pixel<T> oldpix;
  uint32_t fromX = x;
  uint32_t toX = x + xrange;
  uint32_t atY = y;

  averageXSequence(x, toX, y, oldpix);
  newpixel.i = (((uint8_t) (((float)colors) * (oldpix.i / 255.0f))) / ((float)colors)) * 255;
  uint8_t quant_error = oldpix.i - newpixel.i;
  if(quant_error > 0) {
    writeXSequence(x, toX, y, newpixel);

    uint8_t four = (4 * quant_error) >> 4;
    uint8_t three = (3 * quant_error) >> 4;
    uint8_t two = four >> 1;
    uint8_t one = two >> 1;

    fromX = x + xrange;
    toX = fromX + xrange;

    if (toX < w) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, four);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }

    fromX = x + (xrange * 2);
    toX = fromX + xrange;

    if (toX < w) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, three);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }

    fromX = x - (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    if (x >= (xrange * 2) && y >= 1) {
      averageXSequence(fromX, toX, atY, oldpix);
      oldpix.i = add(oldpix.i, one);
      writeXSequence(fromX, toX, atY, oldpix);
    }

    fromX = x -  xrange;
    toX = fromX + xrange;
    atY = y - 1;

    if (x >= xrange && y >= 1) {
      averageXSequence(fromX, toX, atY, oldpix);
      oldpix.i = add(oldpix.i, two);
      writeXSequence(fromX, toX, atY, oldpix);
    }

    fromX = x;
    toX = fromX + xrange;
    atY = y - 1;

    if (toX < w && y >= 1) {
      averageXSequence(x, toX, atY, oldpix);
      oldpix.i = add(oldpix.i, three);
      writeXSequence(x, toX, atY, oldpix);
    }

    fromX = x + xrange;
    toX = fromX + xrange;
    atY = y - 1;

    if (toX < w && y >= 1) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, two);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }

    fromX = x + (xrange * 2);
    toX = fromX + xrange;
    atY = y - 1;

    if (toX < w && y >= 1) {
      averageXSequence(x + xrange, toX + xrange, atY, oldpix);
      oldpix.i = add(oldpix.i, one);
      writeXSequence(x + xrange, toX + xrange, atY, oldpix);
    }
  }
}
*/

void Dither::reduce(Pixel<uint8_t>& pix, const uint8_t colors) {
  float scaled = (float)round( colors * (float)(pix.i / 255.0f) );
  pix.i = round((float)(scaled / colors) * 255);
}

//FIXME how to guarantee T is unsigned and carry signed?
void Dither::add(Pixel<uint8_t>& pix, const int8_t carry) {
  uint8_t sum = pix.i + carry;

  //overflow?
  if(carry > 0 && sum < pix.i)
    sum = 255;
  else if(carry < 0 && sum > pix.i)
    sum = 0;

  pix.i = sum;
}
