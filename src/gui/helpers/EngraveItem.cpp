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

#include "EngraveItem.hpp"

EngraveItem::EngraveItem(Engraving& engraving) : AbstractCtrlCutItem(), engraving(engraving) {
  init();
}

EngraveItem::EngraveItem(const EngraveItem& engraveItem) : AbstractCtrlCutItem(), engraving(engraveItem.engraving) {
  init();
}

void EngraveItem::init() {
  QGraphicsItemGroup::setFlags(QGraphicsItem::ItemIsSelectable | 
                               QGraphicsItem::ItemIsMovable);
  GrayscaleImage gs = engraving.getImage();
  QImage& img = QtMake::make_QImage(gs);
  img.bits();
  QPixmap pixmap = QPixmap::fromImage(img);
  Point pos = engraving.get(EngraveSettings::EPOS);
  this->setPos(QPointF(pos.x, pos.y));
  if (!pixmap.isNull()) {
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap, this);
    QGraphicsItemGroup::addToGroup(pixmapItem);
  }
}

