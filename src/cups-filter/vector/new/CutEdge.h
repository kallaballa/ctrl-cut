#ifndef EDGEPROPERTY_H_
#define EDGEPROPERTY_H_

#include <stdint.h>
#include <utility>
#include <boost/utility.hpp>

class LaserSettings {
public:
  int power;
  int speed;
  int frequency;

  LaserSettings(int power, int speed, int frequency): power(power), speed(speed), frequency(frequency) {}
};


enum edge_lasersettings_t { edge_lasersettings };

typedef boost::property<edge_lasersettings_t, LaserSettings> EdgeProperty;

namespace boost {
  BOOST_INSTALL_PROPERTY(edge, lasersettings);
}

#endif /* EDGEPROPERTY_H_ */
