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

#include "Commands.hpp"
#include "CtrlCutScene.hpp"
#include "ImportDialog.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include <Document.hpp>
#include <cut/operations/Reduce.hpp>
#include <svg/SvgWriter.hpp>

#include <QtGui>

 CtrlCutUndo::CtrlCutUndo(CtrlCutScene* scene, QUndoCommand *parent)
     : QUndoCommand(parent),  scene(scene), before(NULL), after(NULL) {
   before = this->scene->getDocumentHolder();
   after = NULL;
 }

 void CtrlCutUndo::undo() {
   scene->attachDocumentHolder(before);
   scene->update();
 }

 void CtrlCutUndo::redo() {
   if(after == NULL) {
     after = DocumentHolderPtr(new DocumentHolder(*this->scene->getDocumentHolder().get()));
     scene->attachDocumentHolder(after);
     modify();
   } else {
     scene->attachDocumentHolder(after);
   }
   scene->update();
 }

 MoveCommand::MoveCommand(CtrlCutScene* scene, QGraphicsItem *item, const QPointF &oldPos,
                  QUndoCommand *parent)
     : QUndoCommand(parent),  scene(scene)
 {
     qraphicsItem = item;
     newPos = item->pos();
     myOldPos = oldPos;
 }
/*
 bool MoveCommand::mergeWith(const QUndoCommand *command)
 {
     const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
     QGraphicsItem *item = moveCommand->qraphicsItem;

     if (qraphicsItem != item)
     return false;

     newPos = item->pos();
     setText(QObject::tr("Move %1")
         .arg(createCommandString(qraphicsItem, newPos)));
     this->scene->update();
     return true;
 }
*/
 void MoveCommand::undo()
 {
     qraphicsItem->setPos(myOldPos);
     this->scene->update();
     setText(QObject::tr("Move %1")
         .arg(createCommandString(qraphicsItem, newPos)));
 }

 void MoveCommand::redo()
 {
     qraphicsItem->setPos(newPos);
     this->scene->update();
     setText(QObject::tr("Move %1")
         .arg(createCommandString(qraphicsItem, newPos)));
 }

 DeleteCommand::DeleteCommand(CtrlCutScene* scene, QUndoCommand *parent) :
   CtrlCutUndo(scene,parent) {
   setText("Delete item");
 }

 void DeleteCommand::modify() {
   foreach(QGraphicsItem* graphicsItem, scene->selectedItems()) {
     CutItem* ci;
     EngraveItem* ei;

     // FIXME: Handle Group items
     if((ci = dynamic_cast<CutItem* >(graphicsItem))) {
       scene->remove(*ci);
     } else if((ei = dynamic_cast<EngraveItem* >(graphicsItem))) {
       scene->remove(*ei);
     }
   }
 }

 GroupCommand::GroupCommand(CtrlCutScene* scene, QUndoCommand *parent) :
   QUndoCommand(parent), scene(scene) {
   setText("Group items");
 }

 CtrlCutGroupItem *GroupCommand::groupItems(CtrlCutScene* scene, 
                                            QList<QGraphicsItem*> items)
 {
   scene->clearSelection();
   
   CtrlCutGroupItem *group = new CtrlCutGroupItem; // New top-level group

   // Calculate scene brect of all childen.
   // We can use the scene brect since we're adding a top-level object
   QRectF childrenRect;
   foreach (QGraphicsItem *item, items) {
     childrenRect = childrenRect.united(item->sceneBoundingRect());
   }
   
   foreach (QGraphicsItem *item, items) {
     if (AbstractCtrlCutItem *ccitem = dynamic_cast<AbstractCtrlCutItem *>(item)) {
       // Modify item position. This is important to make the group
       // brect calculate properly; it's only updated by addToGroup()
       scene->remove(*ccitem);
       ccitem->setPos(ccitem->pos() - childrenRect.topLeft());
       group->addToGroup(ccitem);
     }
   }
   
   group->setPos(childrenRect.topLeft());
   
   scene->add(*group);
   group->setSelected(true);
   return group;
 }

 void GroupCommand::redo() {
   this->group = groupItems(this->scene, this->scene->selectedItems());
 }

 void GroupCommand::undo() {
   UnGroupCommand::ungroup(this->scene, this->group);
 }

 UnGroupCommand::UnGroupCommand(CtrlCutScene* scene, QUndoCommand *parent) :
   QUndoCommand(parent), scene(scene) {
   setText("Ungroup items");
 }

 QList<QGraphicsItem*> UnGroupCommand::ungroup(CtrlCutScene* scene, 
                                               CtrlCutGroupItem *group)
 {
   scene->clearSelection();

   QList<QGraphicsItem*> items = group->childItems();
   scene->remove(*group);
   foreach(QGraphicsItem *ci, items) {
     group->removeFromGroup(ci);
     scene->add(*dynamic_cast<AbstractCtrlCutItem*>(ci));
     ci->setSelected(true);
   }
   return items;
 }

 void UnGroupCommand::redo() {
   this->items = ungroup(this->scene, dynamic_cast<CtrlCutGroupItem*>(this->scene->selectedItems()[0]));
 }

 void UnGroupCommand::undo() {
   GroupCommand::groupItems(this->scene, this->items);
 }

 OpenCommand::OpenCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent) :
   CtrlCutUndo(scene, parent), filename(filename) {
   setText("Open " + QFileInfo(filename).fileName());
 }

 void OpenCommand::modify() {
   this->scene->open(filename);
   this->scene->getDocumentHolder()->filename = filename;
 }

 SaveCommand::SaveCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent) :
     CtrlCutUndo(scene, parent), filename(filename) {
   setText("Save");
 }

 void SaveCommand::modify() {
   typedef DocumentSettings DS;
   Document& doc = *this->scene->getDocumentHolder()->doc;
   doc.put(DS::TITLE, QFileInfo(filename).baseName().toStdString());
   SvgWriter svgW(doc.get(DS::WIDTH).in(PX), doc.get(DS::HEIGHT).in(PX), doc.get(DS::RESOLUTION), doc.get(DS::TITLE), filename.toStdString().c_str());
   svgW.write(doc, "stroke:rgb(255,0,0);stroke-width:5;");
   this->scene->getDocumentHolder()->filename = filename;
 }

ImportCommand::ImportCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent) :
  CtrlCutUndo(scene, parent), filename(filename) {
  if (!filename.isEmpty()) {
    setText("Import " + QFileInfo(filename).fileName());
  }
}

void ImportCommand::modify() {
  ImportDialog imd;
  if (imd.exec() == QDialog::Accepted) {
    bool loadVector = imd.isVectorDataEnabled();
    bool loadRaster = imd.isRasterDataEnabled();
    this->scene->load(filename, loadVector, loadRaster);
  }
}
/*
 SimplifyCommand::SimplifyCommand(CtrlCutScene* scene, QUndoCommand *parent) :
   QUndoCommand(parent), scene(scene) {
   foreach(QGraphicsItem* item, scene->selectedItems()) {
     CutItem* ci;
     if((ci = dynamic_cast<CutItem* >(item))) {
       oldCutItems.append(ci);
     }
   }
   setText("Simplify");
 }

 void SimplifyCommand::undo() {
   foreach(CutItem* ci, newCutItems) {
     this->scene->remove(*ci);
   }

   foreach(CutItem* ci, oldCutItems) {
     this->scene->add(*ci);
   }

   this->scene->update();
 }

 void SimplifyCommand::redo() {
   if(newCutItems.empty()) {
     foreach(CutItem* ci, oldCutItems) {
       ci->commit();
       //FIXME memory leak
       std::cerr << ci->cut.get(CutSettings::CPOS) << std::endl;
       CutPtr* newCut = new CutPtr(ci->cut.settings);
       std::cerr << newCut->get(CutSettings::CPOS) << std::endl;
       reduce(ci->cut, *newCut, Distance(1, MM,75).in(PX));
       CutItem* newItem = new CutItem(*newCut);
       newCutItems.append(newItem);
     }
   }

   foreach(CutItem* ci, oldCutItems) {
     this->scene->remove(*ci);
   }

   foreach(CutItem* ci, newCutItems) {
     this->scene->add(*ci);
   }

   this->scene->update();
 }
*/
LowerItemCommand::LowerItemCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    CtrlCutUndo(scene, parent) {
  setText("Lower item");
}

void LowerItemCommand::modify() {
  QList<QGraphicsItem *> items = scene->selectedItems();
  qreal selZ = items[0]->zValue();
  qreal z;
  qreal belowZ = selZ;

  foreach (QGraphicsItem *i, scene->items()) {
    z = i->zValue();
    if ((z > belowZ || belowZ == selZ) && z < selZ) {
      belowZ = z;
    }
  }

  items[0]->setZValue(belowZ-1);
}

RaiseItemCommand::RaiseItemCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    CtrlCutUndo(scene, parent) {
  setText("Raise item");
}

void RaiseItemCommand::modify() {
  QList<QGraphicsItem *> items = scene->selectedItems();
  qreal selZ = items[0]->zValue();
  qreal z;
  qreal aboveZ = selZ;

  foreach (QGraphicsItem *i, scene->items()) {
    z = i->zValue();
    if ((z < aboveZ || aboveZ == selZ) && z > selZ) {
      aboveZ = z;
    }
  }

  items[0]->setZValue(aboveZ+1);
}

LowerItemToBottomCommand::LowerItemToBottomCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    CtrlCutUndo(scene, parent) {
  setText("Lower item to bottom");
}

void LowerItemToBottomCommand::modify() {
  QList<QGraphicsItem *> items = scene->selectedItems();
  qreal minz = items[0]->zValue();

  foreach (QGraphicsItem *i, scene->items()) {
    if (i->zValue() > -9999)
      minz = std::min(minz, i->zValue());
  }

  items[0]->setZValue(minz-1);
}

RaiseItemToTopCommand::RaiseItemToTopCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    CtrlCutUndo(scene, parent) {
  setText("Raise item to top");
}

void RaiseItemToTopCommand::modify() {
  QList<QGraphicsItem *> items = scene->selectedItems();
  qreal maxz = items[0]->zValue();

  foreach (QGraphicsItem *i, scene->items()) {
    if (i->zValue() > -9999)
      maxz = std::max(maxz, i->zValue());
  }

  items[0]->setZValue(maxz+1);
}


PasteCommand::PasteCommand(CtrlCutScene* scene, QUndoCommand *parent) : scene(scene) {
  setText("Paste");
}

void PasteCommand::undo() {
  if(!itemsAdded.empty()) {
    foreach (AbstractCtrlCutItem *item, itemsAdded) {
      this->scene->remove(*item);
    }
  }
}
void PasteCommand::redo() {
  if(itemsAdded.empty()) {
    foreach (AbstractCtrlCutItem *item, this->scene->itemClipboard) {
      itemsAdded.append(item->clone());
    }

    foreach (AbstractCtrlCutItem *item, itemsAdded) {
      this->scene->add(*item);
    }
  } else {
    foreach (AbstractCtrlCutItem *item, itemsAdded) {
      this->scene->add(*item);
    }
  }
}

MoveToOriginCommand::MoveToOriginCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    CtrlCutUndo(scene, parent) {
  setText("Paste");
}

void MoveToOriginCommand::modify() {
  qreal minx = std::numeric_limits<qreal>().max();
  qreal miny = std::numeric_limits<qreal>().max();
  foreach (QGraphicsItem *item, this->scene->selectedItems()) {
    const QPointF pos = item->pos();
    minx = std::min(pos.x(), minx);
    miny = std::min(pos.y(), miny);
  }

  foreach (QGraphicsItem *item, this->scene->selectedItems()) {
    const QPointF pos = item->pos();
    item->setPos(pos.x() - minx, pos.y() - miny);
  }
}

QString createCommandString(QGraphicsItem *item, const QPointF &pos) {
  return QObject::tr("Pos (%2, %3)").arg(pos.x()).arg(pos.y());
}
