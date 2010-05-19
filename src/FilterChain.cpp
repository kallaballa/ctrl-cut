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
  this->chain.push_back(new TPFilter());
  this->chain.push_back(new TAFilter());

  this->chain.push_back(new SplitIntersectionsFilter());
  this->chain.push_back(new FindPolylinesFilter());
  this->chain.push_back(new DeonionFilter());
  this->chain.push_back(new SerializeOnionSkinsFilter());
}

FilterChain::~FilterChain() {
  // TODO Auto-generated destructor stub
}

bool FilterChain::evaluate()
{
  set<FilterID> evalPreSet;
  set<FilterID> evalPostSet;

  //TODO env and conf evaluation not implemented yet
  for (FilterList::iterator it = this->chain.begin(); it
         != this->chain.end(); it++) {
    Filter* rf = *it;

    for (FilterIDList::iterator it_cc = rf->preFilters.begin(); it_cc
           != rf->preFilters.end(); it_cc++) {
      FilterID id = *it_cc;

      if (evalPreSet.find(id) == evalPreSet.end())
        return false;
    }

    for (FilterIDList::iterator it_f = rf->provides.begin(); it_f
           != rf->provides.end(); it_f++) {
      evalPreSet.insert(*it_f);
      evalPostSet.erase(*it_f);
    }

    for (FilterIDList::iterator it_f = rf->postFilters.begin(); it_f
           != rf->postFilters.end(); it_f++)
      evalPostSet.insert(*it_f);
  }

  if (evalPostSet.size() > 0)
    return false;

  return true;
}

void FilterChain::filterPass(LaserPass *pass) {
  for (FilterList::const_iterator it = this->chain.begin(); it != this->chain.end(); it++) {
    Filter *f = *it;
    if (f->supports(pass)) f->filter(pass);
  }
}
