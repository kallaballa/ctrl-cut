#ifndef NVERTEX_H_
#define NVERTEX_H_

#include <stdint.h>
#include <utility>
#include "CutPoint2D.h"

enum vertex_point2d_t { vertex_point2d };

typedef boost::property<vertex_point2d_t, CutPoint2D> VertexProperty;

namespace boost {
  BOOST_INSTALL_PROPERTY(vertex, point2d);
}

#endif
