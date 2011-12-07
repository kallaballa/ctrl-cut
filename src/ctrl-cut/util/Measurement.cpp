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

#include "Measurement.h"

const Measurement Measurement::convert(const Unit& target, const uint16_t& dpi) const {
  double spx = -1;

  Measurement convertee(-1, target);
  if (this->unit == PX)
    spx = this->value;
  else if (this->unit == MM)
    spx = this->value * (dpi / MM_TO_INCH);
  else if (this->unit == IN)
    spx = this->value * dpi;
  else
    assert(false);

  if (target == PX)
    convertee.value = spx;
  else if (target == MM)
    convertee.value = spx / (dpi / MM_TO_INCH);
  else if (target == IN)
    convertee.value = spx / dpi;
  else
    assert(false);

  return convertee;
}

const double Measurement::in(const Unit& target, const uint16_t& dpi) const {
  return this->convert(target, dpi).value;
}
