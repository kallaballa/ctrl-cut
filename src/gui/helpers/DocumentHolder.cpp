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
#include "DocumentHolder.hpp"
#include "Document.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"

DocumentHolder::DocumentHolder() : doc(NULL){

}

DocumentHolder::DocumentHolder(const DocumentHolder& other) {
  DocumentHolder::operator=(other);
}

void DocumentHolder::operator=(const DocumentHolder& other) {
  //FIXME memory leak
  this->doc = new Document(*other.doc);
  this->items = other.items;
  this->filename = other.filename;
}

void DocumentHolder::add(AbstractCtrlCutItem &item)
{
  addItem(item);
  addToDocument(item);
}

void DocumentHolder::addItem(AbstractCtrlCutItem &item)
{
  this->items.append(&item);
}

void DocumentHolder::addToDocument(AbstractCtrlCutItem &item)
{
  if (CutItem *cutItem = dynamic_cast<CutItem *>(&item)) {
    this->doc->push_back(cutItem->cut);
  }
  else if (EngraveItem *engraveItem = dynamic_cast<EngraveItem *>(&item)) {
    this->doc->push_back(&engraveItem->engraving);
  }
  else if (CtrlCutGroupItem *groupItem = dynamic_cast<CtrlCutGroupItem *>(&item)) {
    foreach(QGraphicsItem *chitem, groupItem->childItems()) {
      addToDocument(*dynamic_cast<AbstractCtrlCutItem*>(chitem));
    }
  }
}

void DocumentHolder::remove(AbstractCtrlCutItem &item)
{
  this->items.removeOne(&item);
  removeFromDocument(item);
}

void DocumentHolder::removeFromDocument(AbstractCtrlCutItem &item)
{
  if (CutItem *cutItem = qgraphicsitem_cast<CutItem *>(&item)) {
    this->doc->remove(cutItem->cut);
  }
  else if (EngraveItem *engraveItem = dynamic_cast<EngraveItem *>(&item)) {
    this->doc->remove(&engraveItem->engraving);
  }
  else if (CtrlCutGroupItem *groupItem = dynamic_cast<CtrlCutGroupItem *>(&item)) {
    foreach(QGraphicsItem *chitem, groupItem->childItems()) {
      removeFromDocument(*dynamic_cast<AbstractCtrlCutItem*>(chitem));
    }
  }
}
