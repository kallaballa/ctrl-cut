#include "TAFilter.h"

using std::list;

void TAFilter::filter(Raster *raster)
{
  list<PixelBox*> markedCells;
  list<PixelBox*>::iterator it_c;
  PixelBox* c;
  BBox newTileDim;
  list<DownSample*>::iterator it_tp;
  DownSample* tp;

  for (it_tp = raster->grids.begin(); it_tp
         != raster->grids.end(); it_tp++) {
    tp = *it_tp;

    markedCells = tp->getPixels();
    bool startNewTile = true;

    for (it_c = markedCells.begin(); it_c != markedCells.end(); it_c++) {
      c = *it_c;

      //cerr << "Cell: " << c->ul_x << "\t| "  << c->ul_y << "\t| " << c->lr_x << "\t| " << c->lr_y << endl;
      if (startNewTile) {
        newTileDim.ul_x = c->ul_x;
        newTileDim.ul_y = c->ul_y;
        newTileDim.lr_x = c->lr_x;
        newTileDim.lr_y = c->lr_y;
        startNewTile = false;
      } else {
        newTileDim.adjustTo(c);
      }
    }
    MMapImage* tile = raster->sourceImage->tile(newTileDim.ul_x, newTileDim.ul_y, newTileDim.lr_x - newTileDim.ul_x, newTileDim.lr_y - newTileDim.ul_y);
    raster->tiles.push_back(tile);
    startNewTile = true;
  }

#ifdef DEBUG
  list<Tile*>::iterator it_t;
  Image assembledImage(raster->sourceImage->w(), raster->sourceImage->h());
  assembledImage.fill(255);

  for (it_t = raster->tiles.begin(); it_t != raster->tiles.end(); it_t++) {
    Tile* t = *it_t;
    assembledImage.draw_image(t->offsetX(),t->offsetY(), *t);
  }
  assembledImage.save_png("/tmp/assembled.png");
#endif
}
