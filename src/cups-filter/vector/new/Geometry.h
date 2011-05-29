#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <stdint.h>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <vector>

typedef boost::geometry::point<int32_t, 2, boost::geometry::cs::cartesian> Point;
typedef boost::geometry::linear_ring<Point> LinearRing;
typedef boost::geometry::linestring<Point> LineString;

// super type for LinearRing and LineString
typedef std::vector<Point> PolyLine;

#endif /* GEOMETRY_H_ */
