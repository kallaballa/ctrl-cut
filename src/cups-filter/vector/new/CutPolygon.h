#ifndef CUTPOLYGON_H_
#define CUTPOLYGON_H_

#include <boost/geometry/geometry.hpp>

#include "CutPoint2D.h"

typedef boost::geometry::polygon<CutPoint2D> Polygon;

#endif /* CUTPOLYGON_H_ */
