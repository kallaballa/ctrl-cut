#ifndef CUTPOLYLINE_H_
#define CUTPOLYLINE_H_

#include <boost/geometry/geometry.hpp>

#include "CutPoint2D.h"

typedef boost::geometry::linestring<CutPoint2D> Polyline;

#endif /* CUTPOLYLINE_H_ */
