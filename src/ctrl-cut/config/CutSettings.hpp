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

#ifndef CUT_SETTINGS_H_
#define CUT_SETTINGS_H_

#include "DocumentSettings.hpp"
#include "cut/geom/Geometry.hpp"

class CutSettings : public Settings
{
public:
  enum Optimize {
    SIMPLE,
    INNER_OUTER,
    SHORTEST_PATH
  };

  static Key<Optimize> OPTIMIZE;
  static Key<bool> CLIP;
  static Key<Point> CPOS;
  static Key<uint16_t> CSPEED;
  static Key<uint16_t> CPOWER;
  static Key<uint16_t> FREQUENCY;

  CutSettings() : Settings() {}
  CutSettings(DocumentSettings& docSettings) : Settings(docSettings) {
    this->put(CutSettings::OPTIMIZE, CutSettings::INNER_OUTER);
    this->put(CutSettings::CSPEED, 33);
    this->put(CutSettings::CPOWER, 80);
    this->put(CutSettings::FREQUENCY, 5000);
    this->put(CutSettings::CPOS, Point());
  }
  CutSettings(const CutSettings& settings) : Settings(settings) {}
  ~CutSettings() {}
};

#endif /* CUT_SETTINGS_H_ */
