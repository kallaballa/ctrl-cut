

#ifndef DITHER_H_
#define DITHER_H_

#include <util/2D.hpp>
#include <engrave/image/Image.hpp>
#include <config/EngraveSettings.hpp>

#include <stdlib.h>
#include <assert.h>

class Dither {
public:
  GrayscaleImage& img;

  Dither(GrayscaleImage& img): img(img) {}
  virtual ~Dither(){};

  virtual void ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors = 1) = 0;

  BitmapImage dither(Point translate);
  void reduce(Pixel<uint8_t>& pix, const uint8_t colors);
  void add(Pixel<uint8_t>& pix, const int8_t carry) const;
  void carryOver(const uint32_t x, const uint32_t y, const int8_t carryover, Pixel<uint8_t>& p);
  static Dither& create(GrayscaleImage& img, EngraveSettings::Dithering dithering);
};

#endif /* DITHER_H_ */
