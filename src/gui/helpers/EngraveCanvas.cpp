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

#include "EngraveCanvas.h"
#include <qpainter.h>
EngraveCanvas::EngraveCanvas(Coord_t maxWidth) :
  AbstractCtrlCutItem(), maxWidth(maxWidth), startX(-1), currentX(-1), currentY(-1), currentRow(NULL) {
  QGraphicsItemGroup::setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void EngraveCanvas::nextRow(Coord_t x, Coord_t y) {
  if(currentRow) {
    if (!currentRow->isNull()) {
      QPixmap* copy = new QPixmap(*currentRow);
      QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(*copy);
     // pixmapItem->setPos(x,y);
      QGraphicsItemGroup::addToGroup(pixmapItem);
    }
  } else
    currentRow = new QPixmap(21600, 1);

  startX = x;
  currentY = y;
}

void EngraveCanvas::drawPixel(Coord_t x, Coord_t y, uint8_t r,uint8_t g,uint8_t b) {
  if(y != currentY) {
    nextRow(x,y);
  }

  if(x - startX >= 0) {
    QPainter painter(currentRow);
    painter.setPen(Qt::green);
    painter.drawPoint(x - startX, 0);
  }
  currentY = y;
}
