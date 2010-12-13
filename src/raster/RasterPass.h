#ifndef RASTERPASS_H_
#define RASTERPASS_H_

#include "LaserPass.h"

class RasterPass: public LaserPass
{
 public:
  static RasterPass *createFromFile(const string &filename);

 private:
  void serializeTileTo(Tile* tile, ostream &out);
 public:
  Image* sourceImage;
  list<Tile*> tiles;
  list<TilePartitioner*> partitioners;

  RasterPass(Image* sourceImage) {
    this->sourceImage = sourceImage;
  }

 RasterPass(pass_params params) : LaserPass(params) {}
  virtual ~RasterPass() {}

  void addSourceImage(Image* img);
  void addTile(Tile* tile);
  void serializeTo(ostream &out);
};

#endif
