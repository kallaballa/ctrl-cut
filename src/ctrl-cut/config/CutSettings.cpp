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
#include "CutSettings.h"

CutSettings::Key CutSettings::OPTIMIZE = "optimize";
CutSettings::Key CutSettings::CLIP = "CLIP";
CutSettings::Key CutSettings::CSPEED = "speed";
CutSettings::Key CutSettings::CPOWER = "power";
CutSettings::Key CutSettings::FREQUENCY = "frequency";
CutSettings::Key CutSettings::REDUCE = "reduce";

void CutSettings::resetToDefaults() {
  (*this)[OPTIMIZE] = INNER_OUTER;
  (*this)[CSPEED] = 33;
  (*this)[CPOWER] = 80;
  (*this)[FREQUENCY] = 5000;
  (*this)[REDUCE] = 0.0f;
}

/*!
 * Perform range validation checks on the major global variables to ensure
 * their values are sane. If values are outside accepted tolerances then modify
 * them to be the correct value.
 *
 * @return Nothing
 */
void CutSettings::rangeCheck() {
  clip(FREQUENCY,10, 5000);
  clip(CPOWER,0, 100);
  clip(CSPEED,1, 100);
}
