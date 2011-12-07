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
;
using std::string;
using std::stringstream;
using std::pair;

enum Unit {
   PX, MM, IN
 };

class Measurement {
public:
  static const double MM_TO_INCH = 25.4;
  double value;
  Unit unit;

  Measurement(double value, Unit unit) : value(value), unit(unit) {};
  virtual ~Measurement() {};
  const Measurement convert(const Unit& target, const uint16_t& dpi=72) const;
  const double in(const Unit& target, const uint16_t& dpi=72) const;
};


#endif /* UNITS_H_ */
