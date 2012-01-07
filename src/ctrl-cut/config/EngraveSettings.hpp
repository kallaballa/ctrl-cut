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

#ifndef ENGRAVE_SETTINGS_H_
#define ENGRAVE_SETTINGS_H_


#include <stdint.h>
#include "cut/geom/Geometry.hpp"
#include "config/DocumentSettings.hpp"

class EngraveSettings : public Settings
{
public:
  enum Dithering {
    DEFAULT_DITHERING,
    BAYER,
    FLOYD_STEINBERG,
    JARVIS,
    BURKE,
    STUCKI,
    SIERRA2,
    SIERRA3
  };

  enum Direction {
    TOPDOWN = 0,
    BOTTOMUP = 1
  };

  static Key<Dithering> DITHERING;
  static Key<Direction> DIRECTION;

  static Key<Point> EPOS;
  static Key<uint16_t> EPOWER;
  static Key<uint16_t> ESPEED;

  EngraveSettings(const DocumentSettings& docSettings) : Settings(docSettings) {
    this->put(EngraveSettings::DITHERING, EngraveSettings::DEFAULT_DITHERING);
    this->put(EngraveSettings::DIRECTION, EngraveSettings::TOPDOWN);
    this->put(EngraveSettings::EPOWER, 20);
    this->put(EngraveSettings::ESPEED, 100);
    this->put(EngraveSettings::EPOS, Point());
  }
  ~EngraveSettings() {}
};

#endif /* ENGRAVE_SETTINGS_H_ */
