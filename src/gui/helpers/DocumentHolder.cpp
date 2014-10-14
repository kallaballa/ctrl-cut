/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
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
  this->doc = DocumentPtr(new Document(*other.doc));
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
    this->doc->push_back(engraveItem->engraving);
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
  if (CutItem *cutItem = dynamic_cast<CutItem *>(&item)) {
    this->doc->remove(cutItem->cut);
  }
  else if (EngraveItem *engraveItem = dynamic_cast<EngraveItem *>(&item)) {
    this->doc->remove(engraveItem->engraving);
  }
  else if (CtrlCutGroupItem *groupItem = dynamic_cast<CtrlCutGroupItem *>(&item)) {
    foreach(QGraphicsItem *chitem, groupItem->childItems()) {
      removeFromDocument(*dynamic_cast<AbstractCtrlCutItem*>(chitem));
    }
  }
}
