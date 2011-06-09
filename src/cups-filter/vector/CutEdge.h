#ifndef EDGEPROPERTY_H_
#define EDGEPROPERTY_H_

#include <stdint.h>
#include <utility>
#include <boost/utility.hpp>

#include "Geometry.h"

enum edge_string_t { edge_string };
enum edge_geom_t { edge_geom };

typedef boost::property<boost::edge_index_t, int> IndexProperty;
typedef boost::property<edge_string_t, const SegmentString*, IndexProperty>  StringProperty;
typedef boost::property<edge_geom_t, const Segment*, StringProperty>  GeomProperty;

namespace boost {
BOOST_INSTALL_PROPERTY(edge, string);
BOOST_INSTALL_PROPERTY(edge, geom);

}

#endif /* EDGEPROPERTY_H_ */
