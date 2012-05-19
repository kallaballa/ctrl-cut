#include <stdint.h>
#include <iostream>
#include <set>
#include <string>
#include "CImg.hpp"

using namespace cimg_library;

using std::string;
using std::set;
using std::cout;
using std::cerr;
using std::endl;

testimage::testimage() {}
testimage::~testimage() {}
/*
int mainss(int argc, char *argv[]) {
  if(argc != 2) {
    cerr << "Usage: testimage <image>" << endl;
    return 1;
  }

  const char *const fname = argv[1];

  int w = 255*8*2;
  int h = 16;

  CImg<uint8_t> img(w, h, 1, 1, 255);

  for(uint8_t color = 0; color < 255; color++) {
    for(int y = 0; y < h / 2; y++) {
      for(int x = color * 16; x < ((color * 16) + 8); x++) {
        img(x,y) = color;
        img(x + 8 ,y) = 255 - color;
        img(x,y + 8) = 127- color;
        img(x + 8 ,y + 8) = color;
      }
    }
  }

  img.save(fname);
  return 0;
}
*/

int mains(int argc, char *argv[]) {
  if(argc != 2) {
    cerr << "Usage: testimage <image>" << endl;
    return 1;
  }

  const char *const fname = argv[1];

  int w = 255*8;
  int h = 16;

  CImg<uint8_t> img(w, h, 1, 1, 255);

  for(uint8_t color = 0; color < 255; color++) {
    for(int y = 0; y < h; y++) {
      for(int x = color * 8; x < ((color * 8) + 8); x++) {
        img(x,y) = color;
      }
    }
  }

  img.save(fname);
  return 0;
}
