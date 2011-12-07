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
#include "DocumentSettings.h"
#include "util/Measurement.h"
#include "config/LaserCutter.h"

DocumentSettings::Key DocumentSettings::DRIVER = "driver";
DocumentSettings::Key DocumentSettings::USER = "user";
DocumentSettings::Key DocumentSettings::TITLE = "title";
DocumentSettings::Key DocumentSettings::TEMP_DIR = "tmpDir";
DocumentSettings::Key DocumentSettings::DATA_DIR = "dataDir";
DocumentSettings::Key DocumentSettings::BASENAME = "basename";

DocumentSettings::Key DocumentSettings::RESOLUTION = "resolution";
DocumentSettings::Key DocumentSettings::WIDTH = "width";
DocumentSettings::Key DocumentSettings::HEIGHT = "height";
DocumentSettings::Key DocumentSettings::AUTO_FOCUS = "autoFocus";
DocumentSettings::Key DocumentSettings::SCREEN = "screen";

DocumentSettings::Key DocumentSettings::ENABLE_RASTER = "enableRaster";
DocumentSettings::Key DocumentSettings::ENABLE_VECTOR = "enableVector";

void DocumentSettings::resetToDefaults() {
  (*this)[DRIVER] = LaserCutter::UNINITIALIZED;
  (*this)[TEMP_DIR] = string("tmp");
  (*this)[AUTO_FOCUS] = 1;
  (*this)[RESOLUTION] = -1;
  (*this)[SCREEN] = 8;
  (*this)[ENABLE_VECTOR] = true;
  (*this)[ENABLE_RASTER] = false;
  (*this)[WIDTH] = Measurement(-1,PX);
  (*this)[HEIGHT] = Measurement(-1,PX);
}

/*!
 * Perform range validation checks on the major global variables to ensure
 * their values are sane. If values are outside accepted tolerances then modify
 * them to be the correct value.
 *
 * @return Nothing
 */
void DocumentSettings::rangeCheck()
{
 /* int16_t l = 75;
  int16_t h = 1200;
  clip<int16_t>(RESOLUTION,75, h);
  clip(SCREEN,1, 100);
  clip(WIDTH,1, 21600);
  clip(HEIGHT,1, 14400);*/
}
