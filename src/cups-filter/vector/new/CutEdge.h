#ifndef EDGEPROPERTY_H_
#define EDGEPROPERTY_H_

#include <stdint.h>
#include <utility>
#include <boost/utility.hpp>
#include "CutSegment2D.h"

struct LaserSettings {
  int power;
  int speed;
  int frequency;

  LaserSettings(): power(0), speed(0), frequency(0) {};
  LaserSettings(int power, int speed, int frequency): power(power), speed(speed), frequency(frequency) {}
};

enum edge_length_t { edge_length };
enum edge_segment_t { edge_segment };
enum edge_geom_t { edge_geom };
enum edge_cut_t { edge_cut };

typedef boost::property<edge_length_t, double> LengthProperty;
typedef boost::property<edge_segment_t, CutSegment2D, LengthProperty> SegmentProperty;
//FIXME: bool owned_flag
typedef boost::property<edge_geom_t, bool, SegmentProperty> GeomProperty;
typedef boost::property<edge_cut_t, LaserSettings, GeomProperty>  CutProperty;

namespace boost {
BOOST_INSTALL_PROPERTY(edge, length);
BOOST_INSTALL_PROPERTY(edge, segment);
BOOST_INSTALL_PROPERTY(edge, geom);
BOOST_INSTALL_PROPERTY(edge, cut);
}

#endif /* EDGEPROPERTY_H_ */
