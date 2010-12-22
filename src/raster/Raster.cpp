#include "Raster.h"

int tile_cnt = 0;

//TODO rewrite class to be really OO

void Raster::addTile(Tile* tile) {
  this->tiles.push_back(tile);
}

Raster *Raster::createFromFile(const string &filename) {
  return new Raster(new Image(filename.c_str()));
}

