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

#include "Document.h"
#include "CutItem.h"
#include "EngraveItem.h"
#include "../CtrlCutScene.h"

class DocumentItem {
public:
  Document& doc;
  CtrlCutScene& scene;

  DocumentItem(CtrlCutScene& scene, Document& doc) : doc(doc), scene(scene) {
    for(Document::CutIt it = doc.begin_cut(); it != doc.end_cut(); it++) {
      this->scene.addItem(new CutItem(*this, **it));
    }

    for(Document::EngraveIt it = doc.begin_engrave(); it != doc.end_engrave(); it++) {
      this->scene.addItem(new EngraveItem(*this, **it));
    }
  }

  void load(Document& doc) {
    for(Document::CutIt it = doc.begin_cut(); it != doc.end_cut(); it++) {
      this->doc.addCut(*it);
      this->scene.addItem(new CutItem(*this, **it));
    }

    for(Document::EngraveIt it = doc.begin_engrave(); it != doc.end_engrave(); it++) {
      this->doc.addRaster(*it);
      this->scene.addItem(new EngraveItem(*this, **it));
    }
  }

  ~DocumentItem() {};
};

#endif /* DOCUMENTITEM_H_ */
