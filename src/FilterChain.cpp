/*
 * FilterChain.cpp
 *
 *  Created on: 10.11.2009
 *      Author: amir
 */

#include "FilterChain.h"
#include "TPFilter.h"
#include "TAFilter.h"
#include "SplitIntersectionsFilter.h"
#include "FindPolylinesFilter.h"
#include "DeonionFilter.h"
#include "SerializeOnionSkinsFilter.h"
#include <set>
#include <iostream>

using std::set;
using std::endl;
using std::cerr;

FilterChain::FilterChain() {
  //TODO make raster & vector chain configurable
  this->rasterChain.push_back(new TPFilter());
  this->rasterChain.push_back(new TAFilter());

  this->vectorChain.push_back(new SplitIntersectionsFilter());
  this->vectorChain.push_back(new FindPolylinesFilter());
  this->vectorChain.push_back(new DeonionFilter());
  this->vectorChain.push_back(new SerializeOnionSkinsFilter());
}

FilterChain::~FilterChain() {
  // TODO Auto-generated destructor stub
}

bool FilterChain::evaluate() {
  set<FilterID> evalPreSet;
  set<FilterID> evalPostSet;

  //TODO env and conf evaluation not implemented yet
  for (RasterFilterList::iterator it = this->rasterChain.begin(); it
         != this->rasterChain.end(); it++) {
    RasterFilter* rf = *it;

    for (FilterList::iterator it_cc = rf->preFilters.begin(); it_cc
           != rf->preFilters.end(); it_cc++) {
      FilterID id = *it_cc;

      if (evalPreSet.find(id) == evalPreSet.end())
        return false;
    }

    for (FilterList::iterator it_f = rf->provides.begin(); it_f
           != rf->provides.end(); it_f++) {
      evalPreSet.insert(*it_f);
      evalPostSet.erase(*it_f);
    }

    for (FilterList::iterator it_f = rf->postFilters.begin(); it_f
           != rf->postFilters.end(); it_f++)
      evalPostSet.insert(*it_f);
  }

  if (evalPostSet.size() > 0)
    return false;

  return true;
}

void FilterChain::filterRasterPass(RasterPass* rpass) {
  //rpass->tiles.push_back(new Tile(*rpass->sourceImage));
  return;
  for (RasterFilterList::iterator it = rasterChain.begin(); it
         != rasterChain.end(); it++)
    (*it)->filter(rpass);
}

void FilterChain::filterVectorPass(VectorPass* vpass) {
  for (VectorFilterList::iterator it = vectorChain.begin(); it
         != vectorChain.end(); it++)
    (*it)->filter(vpass);
}

