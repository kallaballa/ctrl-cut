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

#include <math.h>
#include <iostream>
#include <algorithm>
#include "util/Logger.h"
#include "vector/Cut.h"
#include "vector/Polyline.h"
#include "raster/DownSample.h"
#include "Flat.h"

Flat::~Flat() {
}
bool closerToOrigin(Polyline* p1, Polyline* p2) {
  return (p1->getBoundingBox()->distanceToOrigin() < p2->getBoundingBox()->distanceToOrigin() );
}

void Flat::filter(Cut *cut) {
  LOG_INFO_MSG("Flat", cut->size());
  sort(cut->begin(), cut->end(), closerToOrigin);

  list<DownSample*> grids;
  list<DownSample*>::iterator it_g;
  map<DownSample*, vector<Polyline*>*> clusters;
  DownSample * grid;
  Polyline* pl;

  for (Cut::iterator it = cut->begin(); it != cut->end(); it++) {
    pl = (*it);
    BBox *bb = pl->getBoundingBox();
    const Point2D &p = bb->ul;
    bool added = false;
    for (it_g = grids.begin(); it_g != grids.end(); it_g++) {
      grid = *it_g;
      if ((added = grid->sample(p))) {
        vector<Polyline* >* vp = (*clusters.find(grid)).second;
        vp->push_back(pl);
        break;
      }
    }

    if (!added) {
      DownSample* gridnew = new DownSample(p, 25, 25, 5, 5, 20);
      grids.push_back(gridnew);

      vector<Polyline* >* vp = new vector<Polyline* >();
      vp->push_back(pl);
      clusters.insert(pair<DownSample*, vector<Polyline* >*> (gridnew, vp));
    }
  }

  vector<Polyline* >* newPolylines = new vector<Polyline* >();
  for (it_g = grids.begin(); it_g != grids.end(); it_g++) {
    grid = *it_g;
    vector<Polyline* >* vp = (*clusters.find(grid)).second;
    newPolylines->insert(newPolylines->end(), vp->begin(), vp->end());
  }

  cut->swap(*newPolylines);
  delete newPolylines;
}
