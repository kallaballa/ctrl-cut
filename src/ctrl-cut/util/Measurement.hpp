/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

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
