/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "DocumentSettings.hpp"
#include "EngraveSettings.hpp"
#include "CutSettings.hpp"
#include "util/Measurement.hpp"

const DocumentSettings::Key<string>  DocumentSettings::DUUID = "duuid";
const DocumentSettings::Key<LaserCutter::Driver>  DocumentSettings::DRIVER = "driver";
const DocumentSettings::Key<string> DocumentSettings::USER = "user";
const DocumentSettings::Key<string> DocumentSettings::TITLE = "title";
const DocumentSettings::Key<string> DocumentSettings::DATA_DIR = "dataDir";
const DocumentSettings::Key<string> DocumentSettings::FILENAME = "filename";

const DocumentSettings::Key<uint16_t> DocumentSettings::RESOLUTION = "resolution";
const DocumentSettings::Key<Distance> DocumentSettings::WIDTH = "width";
const DocumentSettings::Key<Distance> DocumentSettings::HEIGHT = "height";
const DocumentSettings::Key<bool> DocumentSettings::AUTO_FOCUS = "autoFocus";
const DocumentSettings::Key<DocumentSettings::AirAssist> DocumentSettings::AIR_ASSIST = "airAssist";
const DocumentSettings::Key<DocumentSettings::Center> DocumentSettings::CENTER = "center";
const DocumentSettings::Key<uint16_t> DocumentSettings::SCREEN = "screen";

const DocumentSettings::Key<bool> DocumentSettings::ENABLE_ENGRAVING = "enableRaster";
const DocumentSettings::Key<bool> DocumentSettings::ENABLE_CUT = "enableVector";
const DocumentSettings::Key<bool> DocumentSettings::LOAD_CUT = "loadCut";
const DocumentSettings::Key<bool> DocumentSettings::LOAD_ENGRAVING = "loadEngraving";

DocumentSettings::DocumentSettings() : Settings() {
  std::stringstream ss;
  ss << boost::uuids::random_generator()();
  this->put(DUUID, ss.str());
  this->put(DRIVER, LaserCutter::UNINITIALIZED);
  this->put(AUTO_FOCUS, true);
  this->put(AIR_ASSIST, GLOBAL);
  this->put(CENTER, CENTER_NONE);
  this->put(RESOLUTION, 75);
  this->put(SCREEN, 8);
  this->put(ENABLE_CUT, true);
  this->put(ENABLE_ENGRAVING, true);
  this->put(LOAD_CUT, true);
  this->put(LOAD_ENGRAVING, false);

  this->put(WIDTH, Distance(2700,PX,75));
  this->put(HEIGHT, Distance(1800,PX,75));
  //FIXME dont do empty defaults
  this->put(TITLE, "");
  this->put(USER, "");
/*  this->put(EngraveSettings::DITHERING, EngraveSettings::FLOYD_STEINBERG);
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
*/
}
