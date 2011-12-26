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
#include "DocumentItem.h"
#include <qgraphicsitem.h>

CutItem::CutItem(CutModel& cut) : AbstractCtrlCutItem(), cut(cut) {
  QGraphicsItemGroup::setFlags(ItemIsSelectable | ItemIsMovable | ItemIsFocusable);
  for (CutModel::iterator iter = cut.begin(); iter != cut.end(); iter++) {
    Segment& segment = *iter;
    QGraphicsLineItem *line = new QGraphicsLineItem(segment[0][0], segment[0][1], segment[1][0], segment[1][1], this);
    QGraphicsItemGroup::addToGroup(line);
  }
}
