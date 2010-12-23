#ifndef RASTER_H_
#define RASTER_H_

#include "RTypes.h"
#include "DownSample.h"

using namespace std;

class Raster
{
 public:
	static Raster *load(const string &filename);
	Image* sourceImage;
  list<Tile*> tiles;
  list<DownSample*> grids;

  Raster(Image* sourceImage) {
    this->sourceImage = sourceImage;
  }

  virtual ~Raster() {}

  void addSourceImage(Image* img);
  void addTile(Tile* tile);
};

#endif
