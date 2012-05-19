#include <stdint.h>
#include <iostream>
#include <set>
#include <string>
#include "CImg.hpp"
#include "noise.hpp"

using namespace cimg_library;

using std::string;
using std::set;
using std::cout;
using std::cerr;
using std::endl;

noise::noise() {

}

noise::~noise() {

}

int mainss(int argc, char *argv[]) {
  if(argc != 2) {
    cerr << "Usage: noise <image>" << endl;
    return 1;
  }
  const char *const fname = argv[1];

  CImg<uint8_t> img(1000,1000,1,1,255);
  int w = img.width();
  int h = img.height();

  for(int y = 0; y < h; y++) {
    for(int x = 0; x < w; x++) {
      img(x,y) = rand() % 255;
    }
  }

  img.save(fname);
  return 0;
}
