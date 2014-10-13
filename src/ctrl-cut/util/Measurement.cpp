/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "Measurement.hpp"

const double Distance::MM_TO_INCH = 25.4;

const Distance Distance::convert(const Unit& target, const uint16_t dpi) const {
  double spx = -1;

  Distance convertee(-1, target, dpi);
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

double Distance::in(const Unit& target) const {
  return this->convert(target, this->resolution).value;
}
