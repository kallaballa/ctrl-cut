#ifndef NVERTEX_H_
#define NVERTEX_H_

#include <stdint.h>
#include <utility>
#include "Geometry.h"

enum vertex_geom_t { vertex_geom };

typedef boost::property<vertex_geom_t, GeometryMapping> VertexGeomProperty;

namespace boost {
  BOOST_INSTALL_PROPERTY(vertex, geom);
}

#endif
