/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "EngraveItem.hpp"
#include "Qt.hpp"

EngraveItem::EngraveItem(EngravingPtr engraving) : AbstractCtrlCutItem(), engraving(engraving) {
  init();
}

EngraveItem::EngraveItem(const EngraveItem& engraveItem) : AbstractCtrlCutItem(), engraving(EngravingPtr(engraveItem.engraving->clone())) {
  init();
}

void EngraveItem::init() {
  QGraphicsItemGroup::setFlags(QGraphicsItem::ItemIsSelectable | 
                               QGraphicsItem::ItemIsMovable);
  GrayscaleImage gs = engraving->getImage();
  QImage& img = QtMake::make_QImage(gs);
  img.bits();
  QPixmap pixmap = QPixmap::fromImage(img);
  Point pos = engraving->get(EngraveSettings::EPOS);
  this->setPos(QPointF(pos.x, pos.y));
  if (!pixmap.isNull()) {
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap, this);
    QGraphicsItemGroup::addToGroup(pixmapItem);
  }
}

