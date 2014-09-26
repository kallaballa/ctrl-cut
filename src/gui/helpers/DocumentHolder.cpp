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
  this->cutItems = other.cutItems;
  this->engraveItems = other.engraveItems;
}

void DocumentHolder::add(CutItem& cutItem) {
  this->doc->push_back(&cutItem.cut);
  this->cutItems.append(&cutItem);
}

void DocumentHolder::remove(CutItem& cutItem) {
  this->doc->remove(&cutItem.cut);
  this->cutItems.removeOne(&cutItem);
}

void DocumentHolder::add(EngraveItem& engraveItem) {
  this->doc->push_back(&engraveItem.engraving);
  this->engraveItems.append(&engraveItem);
}

void DocumentHolder::remove(EngraveItem& engraveItem) {
  this->doc->remove(&engraveItem.engraving);
  this->engraveItems.removeOne(&engraveItem);
}
