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

#include "Settings.h"

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

  static Key DITHERING;
  static Key DIRECTION;
  static Key EPOWER;
  static Key ESPEED;

  EngraveSettings(): Settings(){}
  ~EngraveSettings() {}
  void resetToDefaults();
  void rangeCheck();
};

#endif /* ENGRAVE_SETTINGS_H_ */
