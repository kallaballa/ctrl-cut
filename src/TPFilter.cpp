/*
 * TPFilter.cpp
 *
 *  Created on: 10.11.2009
 *      Author: amir
 */

#include <list>
#include "TPFilter.h"
#include "LaserPass.h"
#include "Raster.h"
#include "TilePartitioner.h"

using std::list;

void TPFilter::filter(RasterPass* rpass) {
  unsigned char pixel = 0;

  list<TilePartitioner*>::iterator it_tp;
  TilePartitioner * tp;
  int width = rpass->sourceImage->width();
  int height = rpass->sourceImage->height();

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      pixel = (*rpass->sourceImage)(i, j, 0, 0);
      if (pixel < 255) {
        Marker m = { i, j };
        bool added = false;
        for (it_tp = rpass->partitioners.begin(); it_tp
               != rpass->partitioners.end(); it_tp++) {
          tp = *it_tp;

          if ((added = tp->mark(&m)))
            break;
        }

        if (!added) {
          TilePartitioner *tmnew = new TilePartitioner(&m, 25, 25, 5,
                                                       5, 2);
          rpass->partitioners.push_back(tmnew);
        }
      }
    }
  }
}
