
#ifndef UNITS_H_
#define UNITS_H_

#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;
using std::pair;

enum Unit {
   PX, MM, IN
 };

class Distance {
public:
  static const double MM_TO_INCH;
  double value;
  Unit unit;
  uint16_t resolution;

  Distance() : value(0), unit(PX), resolution(0) {};
  Distance(double value, Unit unit, uint16_t resolution) : value(value), unit(unit), resolution(resolution) {};
  virtual ~Distance() {};
  const Distance convert(const Unit& target, const uint16_t dpi) const;
  double in(const Unit& target) const;
  void toJson(std::ostream& os) {
    os << "{ \"value\":" << value << ", \"unit\":";

    if (unit == MM)
      os << "\"mm\"";
    else if (unit == IN)
      os << "\"in\"";
    else if (unit == PX)
      os << "\"px\"";

    os << ", \"resolution\":" << resolution << " }";
  }
};

inline std::ostream& operator<<(std::ostream &os, const Distance& m)  {
  os << m.value;

  if (m.unit == MM)
    os << "mm";
  else if (m.unit == IN)
    os << "in";
  else if (m.unit == PX)
    os << "px";

  return os;
}

inline std::istream& operator>>(std::istream &is, Distance& m)  {
  double value;
  string unit;

  is >> value;
  is >> unit;

  m.value = value;
  if (unit == "mm")
    m.unit = MM;
  else if (unit == "in")
    m.unit = IN;
  else if (unit == "px")
    m.unit = PX;

  return is;
}


#endif /* UNITS_H_ */
