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

#include "DocumentSettings.hpp"
#include "EngraveSettings.hpp"
#include "BitmapSettings.hpp"
#include "CutSettings.hpp"
#include "util/Measurement.hpp"

const DocumentSettings::Key<string>  DocumentSettings::DUUID = "duuid";
const DocumentSettings::Key<LaserCutter::Driver>  DocumentSettings::DRIVER = "driver";
const DocumentSettings::Key<string> DocumentSettings::USER = "user";
const DocumentSettings::Key<string> DocumentSettings::TITLE = "title";
const DocumentSettings::Key<string> DocumentSettings::TEMP_DIR = "tmpDir";
const DocumentSettings::Key<string> DocumentSettings::DATA_DIR = "dataDir";
const DocumentSettings::Key<string> DocumentSettings::FILENAME = "filename";

const DocumentSettings::Key<uint16_t> DocumentSettings::RESOLUTION = "resolution";
const DocumentSettings::Key<Distance> DocumentSettings::WIDTH = "width";
const DocumentSettings::Key<Distance> DocumentSettings::HEIGHT = "height";
const DocumentSettings::Key<bool> DocumentSettings::AUTO_FOCUS = "autoFocus";
const DocumentSettings::Key<uint16_t> DocumentSettings::SCREEN = "screen";

const DocumentSettings::Key<bool> DocumentSettings::ENABLE_ENGRAVING = "enableRaster";
const DocumentSettings::Key<bool> DocumentSettings::ENABLE_CUT = "enableVector";
const DocumentSettings::Key<bool> DocumentSettings::LOAD_CUT = "loadCut";
const DocumentSettings::Key<bool> DocumentSettings::LOAD_ENGRAVING = "loadEngraving";

const DocumentSettings::Key<bool> DocumentSettings::DUMP_XML = "dumpXml";

DocumentSettings::DocumentSettings() : Settings() {
  std::stringstream ss;
  ss << boost::uuids::random_generator()();
  this->put(DUUID, ss.str());
  this->put(DRIVER, LaserCutter::UNINITIALIZED);
  this->put(TEMP_DIR, string("tmp"));
  this->put(AUTO_FOCUS, true);
  this->put(RESOLUTION, 75);
  this->put(SCREEN, 8);
  this->put(ENABLE_CUT, true);
  this->put(ENABLE_ENGRAVING, true);
  this->put(LOAD_CUT, true);
  this->put(LOAD_ENGRAVING, false);

  this->put(WIDTH, Distance(2700,PX,75));
  this->put(HEIGHT, Distance(1800,PX,75));
  this->put(DUMP_XML, false);
  //FIXME dont do empty defaults
  this->put(TITLE, "");
  this->put(USER, "");
  this->put(EngraveSettings::DITHERING, EngraveSettings::FLOYD_STEINBERG);
  this->put(EngraveSettings::DIRECTION, EngraveSettings::TOPDOWN);
  this->put(EngraveSettings::EPOWER, 20);
  this->put(EngraveSettings::ESPEED, 100);
  this->put(EngraveSettings::EPOS, Point());

  this->put(BitmapSettings::BPOS, Point());

  this->put(CutSettings::SORT, CutSettings::INNER_OUTER);
  this->put(CutSettings::CSPEED, 33);
  this->put(CutSettings::CPOWER, 80);
  this->put(CutSettings::FREQUENCY, 5000);
  this->put(CutSettings::CPOS, Point());
}
