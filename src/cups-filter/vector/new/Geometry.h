#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <stdint.h>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <deque>

typedef boost::geometry::point<int32_t, 2, boost::geometry::cs::cartesian> Point;
typedef boost::geometry::segment<Point> Segment;
typedef boost::geometry::segment<const Point> ConstSegment;
typedef boost::geometry::linear_ring<Point, std::deque> LinearRing;
typedef boost::geometry::linestring<Point, std::deque> Linestring;

// super type for LinearRing and LineString
typedef std::deque<Point> PolyLine;

#endif /* GEOMETRY_H_ */
