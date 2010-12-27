#include "Raster.h"

int tile_cnt = 0;

//TODO rewrite class to be really OO

void Raster::addTile(MMapImage* tile) {
  this->tiles.push_back(tile);
}

Raster *Raster::load(const string &filename) {
  return new Raster(new MMapImage(filename.c_str()));
}

