/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
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
	assert(false);
  return NULL;
}

void AbstractCtrlCutItem::setHighlighted(bool enabled) {

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
