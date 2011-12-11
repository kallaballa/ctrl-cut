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
#include "EngraveSettings.h"
#include "CutSettings.h"
#include "util/Measurement.h"
#include "config/LaserCutter.h"

const DocumentSettings::Key<LaserCutter::Driver>  DocumentSettings::DRIVER = "driver";
const DocumentSettings::Key<string> DocumentSettings::USER = "user";
const DocumentSettings::Key<string> DocumentSettings::TITLE = "title";
const DocumentSettings::Key<string> DocumentSettings::TEMP_DIR = "tmpDir";
const DocumentSettings::Key<string> DocumentSettings::DATA_DIR = "dataDir";
const DocumentSettings::Key<string> DocumentSettings::BASENAME = "basename";

const DocumentSettings::Key<uint16_t> DocumentSettings::RESOLUTION = "resolution";
const DocumentSettings::Key<Measurement> DocumentSettings::WIDTH = "width";
const DocumentSettings::Key<Measurement> DocumentSettings::HEIGHT = "height";
const DocumentSettings::Key<bool> DocumentSettings::AUTO_FOCUS = "autoFocus";
const DocumentSettings::Key<uint16_t> DocumentSettings::SCREEN = "screen";

const DocumentSettings::Key<bool> DocumentSettings::ENABLE_RASTER = "enableRaster";
const DocumentSettings::Key<bool> DocumentSettings::ENABLE_VECTOR = "enableVector";

void DocumentSettings::resetToDefaults() {
  this->put(DRIVER, LaserCutter::UNINITIALIZED);
  this->put(TEMP_DIR, string("tmp"));
  this->put(AUTO_FOCUS, true);
  this->put(RESOLUTION, 0);
  this->put(SCREEN, 8);
  this->put(ENABLE_VECTOR, true);
  this->put(ENABLE_RASTER, false);
  this->put(WIDTH, Measurement(-1,PX));
  this->put(HEIGHT, Measurement(-1,PX));
  this->put(EngraveSettings::DITHERING, EngraveSettings::DEFAULT_DITHERING);
  this->put(EngraveSettings::DIRECTION, EngraveSettings::TOPDOWN);
  this->put(EngraveSettings::EPOWER, 20);
  this->put(EngraveSettings::ESPEED, 100);
  this->put(CutSettings::OPTIMIZE, CutSettings::INNER_OUTER);
  this->put(CutSettings::CSPEED, 33);
  this->put(CutSettings::CPOWER, 80);
  this->put(CutSettings::FREQUENCY, 5000);
  this->put(CutSettings::REDUCE, 0.0f);
}
