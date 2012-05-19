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


#ifndef ENGRAVINGCANVAS_H_
#define ENGRAVINGCANVAS_H_

#include "engrave/Engrave.hpp"
#include "config/EngraveSettings.hpp"
#include <qgraphicsitem.h>
#include "helpers/Qt.hpp"

class EngraveCanvas: public AbstractCtrlCutItem {
public:

  EngraveCanvas(Coord_t maxWidth);
  ~EngraveCanvas(){};
  void nextRow(Coord_t x, Coord_t y);
  void drawPixel(Coord_t x, Coord_t y, uint8_t r,uint8_t g,uint8_t b);
  void commit() {};
private:
  Coord_t maxWidth;
  Coord_t startX;
  Coord_t currentX;
  Coord_t currentY;
  QPixmap* currentRow;
};

#endif /* ENGRAVINGCANVAS_H_ */
