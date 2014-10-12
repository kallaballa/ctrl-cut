

#include "DitherBayer.hpp"

static const float bayer_matrix[8][8] = {
    { 0, 32,  8, 40,  2, 34, 10, 42},
    {48, 16, 56, 24, 50, 18, 58, 26},
    {12, 44,  4, 36, 14, 46,  6, 38},
    {60, 28, 52, 20, 62, 30, 54, 22},
    { 3, 35, 11, 43,  1, 33,  9, 41},
    {51, 19, 59, 27, 49, 17, 57, 25},
    {15, 47,  7, 39, 13, 45,  5, 37},
    {63, 31, 55, 23, 61, 29, 53, 21} };

void DitherBayer::ditherPixel(const uint32_t x, const uint32_t y, Pixel<uint8_t>& pix, const uint32_t colors) {
  this->img.readPixel(x, y, pix);
  float scaled = (pix.i / 255.0f) * 63.0f;
  if(scaled < bayer_matrix[x % 8][y % 8])
    pix.i = 0;
  else
    pix.i = 255;
}
