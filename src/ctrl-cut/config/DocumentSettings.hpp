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

#ifndef DOCUMENT_SETTINGS_H_
#define DOCUMENT_SETTINGS_H_

#include "Settings.hpp"
#include "LaserCutter.hpp"

class DocumentSettings : public Settings
{
public:
  // the configured laser cutter hardware
  const static Key<LaserCutter::Driver> DRIVER;
  const static Key<string> USER;
  const static Key<string> TITLE;
  const static Key<string>  TEMP_DIR;
  const static Key<string>  DATA_DIR;
  const static Key<string>  FILENAME;

  const static Key<uint16_t> RESOLUTION;
  const static Key<Distance> WIDTH;
  const static Key<Distance> HEIGHT;
  const static Key<bool> AUTO_FOCUS;
  const static Key<uint16_t> SCREEN;

  const static Key<bool> ENABLE_ENGRAVING;
  const static Key<bool> ENABLE_CUT;
  const static Key<bool> LOAD_CUT;
  const static Key<bool> LOAD_ENGRAVING;
  const static Key<bool> DUMP_XML;

  DocumentSettings();
  ~DocumentSettings() {}
};

#endif /* DOCUMENT_SETTINGS_H_ */
