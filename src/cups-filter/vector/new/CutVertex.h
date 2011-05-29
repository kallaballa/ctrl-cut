#ifndef NVERTEX_H_
#define NVERTEX_H_

#include <stdint.h>
#include <utility>
#include "Geometry.h"

enum vertex_point_t { vertex_point };

typedef boost::property<vertex_point_t, Point> PointProperty;

namespace boost {
  BOOST_INSTALL_PROPERTY(vertex, point);
}

#endif
