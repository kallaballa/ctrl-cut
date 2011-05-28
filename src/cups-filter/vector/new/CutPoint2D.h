#ifndef CUTPOINT2D_H_
#define CUTPOINT2D_H_

#include <stdint.h>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/register/point.hpp>

typedef boost::geometry::point<uint32_t, 2, boost::geometry::cs::cartesian> CutPoint2D;


#endif /* CUTPOINT2D_H_ */
