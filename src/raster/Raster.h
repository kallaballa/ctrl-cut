#ifndef RASTER_H_
#define RASTER_H_
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <list>
#include "boost/format.hpp"

#include "../util/PJL.h"
#include "RTypes.h"
#include "TilePartitioner.h"

using namespace boost;
using namespace std;

class Raster
{
 public:
	static Raster *createFromFile(const string &filename);
	Image* sourceImage;
  list<Tile*> tiles;

  Raster(Image* sourceImage) {
    this->sourceImage = sourceImage;
  }

  virtual ~Raster() {}

  void addSourceImage(Image* img);
  void addTile(Tile* tile);
};

#endif
