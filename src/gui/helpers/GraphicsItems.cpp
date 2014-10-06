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

#include "GraphicsItems.hpp"
#include "CutItem.hpp"
#include "EngraveItem.hpp"

#include <QGraphicsEffect>
#include <QStyleOption>
#include <QPainter>

AbstractCtrlCutItem *AbstractCtrlCutItem::clone() const
{
  //FIXME memory leak
  if (const CutItem *cutItem = dynamic_cast<const CutItem *>(this)) {
    CutItem* ci = new CutItem(*cutItem);
    assert(ci->cut.get() != cutItem->cut.get());
    return ci;
  }
  else if (const EngraveItem *engraveItem = dynamic_cast<const EngraveItem *>(this)) {
    return new EngraveItem(*engraveItem);
  }
  else if (const CtrlCutGroupItem *groupItem = dynamic_cast<const CtrlCutGroupItem *>(this)) {
    return new CtrlCutGroupItem(*groupItem);
  }
}

void AbstractCtrlCutItem::setHighlighted(bool enabled) {

  printf("%p: setHighlighted(%d)\n", this, enabled);
  /*   return;
  // FIXME causes performance issues*/
  QGraphicsColorizeEffect* highlight = new QGraphicsColorizeEffect();
  highlight->setColor(Qt::green);
  if(enabled) {
    if (graphicsEffect() == NULL) setGraphicsEffect(highlight);
  }
  else {
    if(QGraphicsItemGroup::graphicsEffect() != NULL)
      QGraphicsItemGroup::setGraphicsEffect(NULL);
  }
}

void AbstractCtrlCutItem::paint(QPainter *painter, 
                                const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  if (!this->parentItem() && this->isSelected()) {
    QRectF rect = this->childrenBoundingRect();
    printf("paint rect: [%.0f,%.0f,%.0f,%.0f]\n", rect.x(), rect.y(), rect.width(), rect.height());
    painter->drawRect(this->childrenBoundingRect());
  }
}

CtrlCutGroupItem::CtrlCutGroupItem()
{
  init();
}

CtrlCutGroupItem::CtrlCutGroupItem(const CtrlCutGroupItem& groupItem)
{
  foreach(QGraphicsItem *item, groupItem.childItems()) {
    if (AbstractCtrlCutItem *ci = dynamic_cast<AbstractCtrlCutItem *>(item)) {
      this->addToGroup(ci->clone());
    }
  }

  init();
}

void CtrlCutGroupItem::init()
{
  QGraphicsItemGroup::setFlags(ItemIsSelectable | ItemIsMovable);
}

void CtrlCutGroupItem::commit() {
  
}
