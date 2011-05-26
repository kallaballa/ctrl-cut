#ifndef CUTTWOD_H_
#define CUTTWOD_H_

#include <iostream>
#include <utility>
#include <algorithm>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/multi/multi.hpp>

#include "CutEdge.h"
#include "CutVertex.h"
#include "CutPolygon.h"

using boost::geometry::multi_linestring;
using boost::geometry::multi_polygon;

typedef multi_polygon<Polygon> Polygons;

class Cut2D {
private:
  Polygons polygons;

public:
  Cut2D();
  virtual ~Cut2D();
};

#endif
