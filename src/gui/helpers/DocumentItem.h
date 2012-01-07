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


#ifndef DOCUMENTITEM_H_
#define DOCUMENTITEM_H_

#include <vector>
#include "Document.hpp"
#include "CutItem.h"
#include "EngraveItem.h"
#include "../CtrlCutScene.h"
#include "Qt.h"

class DocumentItem : public AbstractCtrlCutItem {
public:
  Document& doc;
  CtrlCutScene& scene;

  DocumentItem(CtrlCutScene& scene, Document& doc) : AbstractCtrlCutItem(), doc(doc), scene(scene) {
    for(Document::CutIt it = doc.begin_cut(); it != doc.end_cut(); it++) {
      this->scene.addItem(new CutItem(**it));
    }

    for(Document::EngraveIt it = doc.begin_engrave(); it != doc.end_engrave(); it++) {
      this->scene.addItem(new EngraveItem(**it));
    }
  }

  void load(Document& doc) {
    for(Document::CutIt it = doc.begin_cut(); it != doc.end_cut(); it++) {
      this->doc.push_back(*it);
      this->scene.addItem(new CutItem(**it));
    }

    for(Document::EngraveIt it = doc.begin_engrave(); it != doc.end_engrave(); it++) {
      this->doc.push_back(*it);
      this->scene.addItem(new EngraveItem(**it));
    }
  }

  ~DocumentItem() {};

  void commit() {
    foreach (QGraphicsItem *sitem, this->scene.items()){
      AbstractCtrlCutItem* ccItem;
      if ((ccItem = dynamic_cast<AbstractCtrlCutItem*> (sitem->parentItem()))) {
        ccItem->commit();
      }
    }
  }
};

#endif /* DOCUMENTITEM_H_ */
