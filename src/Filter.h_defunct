#ifndef FILTER_H_
#define FILTER_H_

//raster filter ids
#define TP_FILTER_ID "TilePartitionerFilter"
#define TA_FILTER_ID "TileAssemblerFilter"

//vector filter ids
#define SI_FILTER_ID "SplitIntersectionsFilter"
#define FP_FILTER_ID "FindPolylinesFilter"
#define SER_PL_FILTER_ID "SerializePolylinesFilter"
#define DO_FILTER_ID "DeonionFilter"
#define SER_OS_FILTER_ID "SerializeOnionSkinsFilter"

#include <set>
#include <string>
#include "LaserPass.h"

using std::list;
using std::string;

typedef string FilterID;
typedef set<FilterID> FilterIDList;

class Filter {
 public:
  FilterIDList provides;
  FilterIDList preFilters;
  FilterIDList postFilters;
  virtual bool supports(LaserPass *p) = 0;
  virtual void filter(LaserPass *p) = 0;
};

class RasterFilter: public Filter {
 public:

  RasterFilter(){}
  virtual ~RasterFilter(){};
  virtual bool supports(LaserPass *p);
  virtual void filter(LaserPass *p) = 0;
};

class VectorFilter: public Filter {
 public:

  VectorFilter(){}
  virtual ~VectorFilter(){};
  virtual bool supports(LaserPass *p);
  virtual void filter(LaserPass *p) = 0;
};

#endif /* FILTER_H_ */
