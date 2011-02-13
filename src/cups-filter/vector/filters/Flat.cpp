/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "util/Logger.h"
#include "vector/VTypes.h"
#include "vector/Edge.h"
#include "vector/Polyline.h"
#include "vector/Cut.h"
#include "Flat.h"
#include "raster/DownSample.h"
#include <algorithm>

Flat::~Flat() {
}
bool closerToOrigin(Polyline* p1, Polyline* p2) {
  return (p1->getBoundingBox()->distanceToOrigin() < p2->getBoundingBox()->distanceToOrigin() );
}

void Flat::filter(Cut *cut) {
  LOG_INFO_MSG("Flat", cut->polylines.size());
  sort(cut->polylines.begin(), cut->polylines.end(), closerToOrigin);

  list<DownSample*> grids;
  list<DownSample*>::iterator it_g;
  map<DownSample*, VecPolyline*> clusters;
  DownSample * grid;
  Polyline* pl;

  for (vector<Polyline*>::iterator it = cut->polylines.begin(); it
      != cut->polylines.end(); it++) {
    pl = (*it);
    Point2D* p;
    BBox* bb = pl->getBoundingBox();
    bool added = false;
    for (it_g = grids.begin(); it_g != grids.end(); it_g++) {
      grid = *it_g;
      p = new Point2D(bb->ul_x, bb->ul_y);
      if ((added = grid->sample(p))) {
        VecPolyline* vp = (*clusters.find(grid)).second;
        vp->push_back(pl);
        break;
      }
    }

    if (!added) {
      DownSample* gridnew = new DownSample(p, 25, 25, 5, 5, 20);
      grids.push_back(gridnew);

      VecPolyline* vp = new VecPolyline();
      vp->push_back(pl);
      clusters.insert(pair<DownSample*, VecPolyline*> (gridnew, vp));
    }
  }

  VecPolyline* newPolylines = new VecPolyline();
  for (it_g = grids.begin(); it_g != grids.end(); it_g++) {
    grid = *it_g;
    VecPolyline* vp = (*clusters.find(grid)).second;
    newPolylines->insert(newPolylines->end(), vp->begin(), vp->end());
  }

  cut->polylines.swap(*newPolylines);
  delete newPolylines;
}
