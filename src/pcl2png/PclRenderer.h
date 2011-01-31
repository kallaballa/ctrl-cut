/*
 * EpilogCUPS - A laser cutter CUPS driver
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

#ifndef PCLRENDERER_H_
#define PCLRENDERER_H_

#include "stdint.h"

class PclToken {
public:
  PclToken() : keysep('\0') {}
  char type;
  char prefix;
  char suffix;
  char keysep; // separate the cmd signature from value/data so it may be interpreted as a string and used as key
  int32_t value;
  unsigned char * data;
};

class PclRenderer {
public:
  PclRenderer();
  virtual ~PclRenderer();
};

#endif /* PCLRENDERER_H_ */
