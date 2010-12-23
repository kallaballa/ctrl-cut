#include "TPFilter.h"

void TPFilter::filter(Raster *raster)
{
  unsigned char pixel = 0;

  list<DownSample*>::iterator it_tp;
  DownSample * tp;
  int width = raster->sourceImage->width();
  int height = raster->sourceImage->height();

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      pixel = (*raster->sourceImage)(i, j, 0, 0);
      if (pixel < 255) {
        Point2D m = { i, j };
        bool added = false;
        for (it_tp = raster->grids.begin(); it_tp
               != raster->grids.end(); it_tp++) {
          tp = *it_tp;

          if ((added = tp->sample(&m)))
            break;
        }

        if (!added) {
          DownSample *tmnew = new DownSample(&m, 25, 25, 5,
                                                       5, 2);
          raster->grids.push_back(tmnew);
        }
      }
    }
  }
}
