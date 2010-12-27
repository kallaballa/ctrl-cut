#ifndef RASTER_H_
#define RASTER_H_

#include "RTypes.h"
#include "DownSample.h"
#include "MMapImage.h"

using namespace std;

class Raster
{
 public:
	static Raster *load(const string &filename);
	MMapImage* sourceImage;
  list<MMapImage*> tiles;
  list<DownSample*> grids;

  Raster(MMapImage* sourceImage) {
    this->sourceImage = sourceImage;
  }

  virtual ~Raster() {}

  void addSourceImage(MMapImage* img);
  void addTile(MMapImage* tile);
};

#endif
