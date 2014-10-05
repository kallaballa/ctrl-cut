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
#include "CutItem.hpp"
#include "PathItem.hpp"
#include "cut/operations/Translate.hpp"
#include <qgraphicsitem.h>
#include <boost/foreach.hpp>

CutItem::CutItem(CutPtr cut) : AbstractCtrlCutItem(), cut(cut) {
  init();
}

CutItem::CutItem(const CutItem& cutItem) : AbstractCtrlCutItem(), cut(CutPtr(new Cut(cutItem.cut->settings))) {
  (*cut.get()) = *cutItem.cut.get();
  init();
}

void CutItem::init() {
  QGraphicsItemGroup::setFlags(ItemIsSelectable | ItemIsMovable);
  this->cut->crop();
  Point pos = this->cut->get(CutSettings::CPOS);
  setPos(QPointF(pos.x, pos.y));

  BOOST_FOREACH(const Path& p, *cut) {
    PathItem* pi = new PathItem(p);
    pi->setPos(QPointF(pos.x, pos.y));
    this->addToGroup(pi);
  }
}
