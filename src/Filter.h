/*
 * RasterFilter.h
 *
 *  Created on: 10.11.2009
 *      Author: amir
 */

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

#include <map>
#include <list>
#include <string>
#include <iostream>
#include "VectorPass.h"
#include "RasterPass.h"

using std::map;
using std::list;
using std::string;
using std::cerr;
using std::endl;

typedef string FilterID;
typedef set<FilterID > FilterList;

class Filter {
 public:
  FilterList provides;
  FilterList preFilters;
  FilterList postFilters;
};

class RasterFilter: public Filter {
 public:

  RasterFilter(){}
  virtual ~RasterFilter(){};
  virtual void filter(RasterPass* p) = 0;
};

class VectorFilter: public Filter {
 public:

  VectorFilter(){}
  virtual ~VectorFilter(){};
  virtual void filter(VectorPass* p) = 0;
};

#endif /* FILTER_H_ */
