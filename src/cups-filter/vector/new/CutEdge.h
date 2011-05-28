#ifndef EDGEPROPERTY_H_
#define EDGEPROPERTY_H_

#include <stdint.h>
#include <utility>
#include <boost/utility.hpp>
#include "CutSegment2D.h"

class LaserSettings {
public:
  int power;
  int speed;
  int frequency;

  LaserSettings(int power, int speed, int frequency): power(power), speed(speed), frequency(frequency) {}
};

enum edge_length_t { edge_length };
enum edge_owned_t { edge_owned };
enum edge_segment_t { edge_segment };
enum edge_geom_t { edge_geom };

enum edge_lasersettings_t { edge_lasersettings };
enum edge_cut_t { edge_cut };

typedef boost::property<edge_length_t, double> LengthProperty;
typedef boost::property<edge_segment_t, CutSegment2D, LengthProperty> SegmentProperty;

typedef boost::property<edge_owned_t, bool> OwnedProperty;
typedef boost::property<edge_geom_t, SegmentProperty, OwnedProperty> GeomProperty;

typedef boost::property<edge_lasersettings_t, LaserSettings> LaserProperty;
typedef boost::property<edge_cut_t, GeomProperty, LaserProperty>  CutProperty;

namespace boost {
BOOST_INSTALL_PROPERTY(edge, length);
BOOST_INSTALL_PROPERTY(edge, owned);
BOOST_INSTALL_PROPERTY(edge, segment);
BOOST_INSTALL_PROPERTY(edge, geom);
BOOST_INSTALL_PROPERTY(edge, lasersettings);
BOOST_INSTALL_PROPERTY(edge, cut);
}

#endif /* EDGEPROPERTY_H_ */
