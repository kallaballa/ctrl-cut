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
 * GNU General Public License for more de0tails.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "CutItem.h"
#include "PathItem.h"
#include "cut/model/Translate.hpp"
#include <qgraphicsitem.h>


CutItem::CutItem(CutModel& cut) : AbstractCtrlCutItem(), cut(cut) {
  QGraphicsItemGroup::setFlags(ItemIsSelectable | ItemIsMovable | ItemIsFocusable);
  Point pos = this->cut.get(CutSettings::CPOS);
  this->setPos(QPointF(pos.x, pos.y));

  BOOST_FOREACH(Path& p, cut) {
    PathItem* pi = new PathItem(p);
    QGraphicsItemGroup::addToGroup(pi);
  }
}

void CutItem::setPos(const QPointF &pos) {
  QGraphicsItem::setPos(pos);
}
