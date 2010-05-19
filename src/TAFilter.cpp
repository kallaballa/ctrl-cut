#include "TAFilter.h"
#include "RasterPass.h"
#include "Raster.h"
#include "TilePartitioner.h"

#include <list>

using std::list;

void TAFilter::filter(LaserPass *pass)
{
  RasterPass* rpass = dynamic_cast<RasterPass*>(pass);

  list<Cell*> markedCells;
  list<Cell*>::iterator it_c;
  Cell* c;
  BBox newTileDim;
  list<TilePartitioner*>::iterator it_tp;
  TilePartitioner * tp;

  for (it_tp = rpass->partitioners.begin(); it_tp
         != rpass->partitioners.end(); it_tp++) {
    tp = *it_tp;

    markedCells = tp->getMarkedCells();
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
    Tile* tile = new Tile(newTileDim.ul_x, newTileDim.ul_y, newTileDim.lr_x
                          - newTileDim.ul_x, newTileDim.lr_y - newTileDim.ul_y);
    tile->fill(255).draw_image(-newTileDim.ul_x, -newTileDim.ul_y,
                               *rpass->sourceImage);
    rpass->tiles.push_back(tile);
    startNewTile = true;
  }

#ifdef DEBUG
  list<Tile*>::iterator it_t;
  Image assembledImage(rpass->sourceImage->width(), rpass->sourceImage->height());
  assembledImage.fill(255);

  for (it_t = rpass->tiles.begin(); it_t != rpass->tiles.end(); it_t++) {
    Tile* t = *it_t;
    assembledImage.draw_image(t->offsetX(),t->offsetY(), *t);
  }
  assembledImage.save_png("/tmp/assembled.png");
#endif
}
