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

#include <QtGui>

 CtrlCutUndo::CtrlCutUndo(CtrlCutScene* scene, QUndoCommand *parent)
     : QUndoCommand(parent),  scene(scene) {
 }


 MoveCommand::MoveCommand(CtrlCutScene* scene, QGraphicsItem *item, const QPointF &oldPos,
                  QUndoCommand *parent)
     : CtrlCutUndo(scene, parent)
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
   CtrlCutUndo(scene,parent), deleted(NULL) {
   setText("Delete item");
 }

 void DeleteCommand::undo() {
   if(deleted)
     scene->add(*deleted);
 }

 void DeleteCommand::redo() {
   if(!deleted) {
     foreach(QGraphicsItem* graphicsItem, scene->selectedItems()) {
       CutItem* ci;
       EngraveItem* ei;

       // FIXME: Handle Group items
       if((ci = dynamic_cast<CutItem* >(graphicsItem))) {
         deleted = ci;
         scene->remove(*ci);
       } else if((ei = dynamic_cast<EngraveItem* >(graphicsItem))) {
         deleted = ei;
         scene->remove(*ei);
       }
     }
   } else
     scene->remove(*deleted);
 }

 GroupCommand::GroupCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    CtrlCutUndo(scene, parent) {
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
    CtrlCutUndo(scene, parent) {
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

ImportCommand::ImportCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent) :
  CtrlCutUndo(scene, parent), filename(filename) {
  if (!filename.isEmpty()) {
    setText("Import " + QFileInfo(filename).fileName());
  }
}

void ImportCommand::undo() {
  for(AbstractCtrlCutItem* item : imported) {
    this->scene->remove(*item);
  }
}
void ImportCommand::redo() {
  ImportDialog imd;
  if (imd.exec() == QDialog::Accepted) {
    bool loadVector = imd.isVectorDataEnabled();
    bool loadRaster = imd.isRasterDataEnabled();
    imported = this->scene->load(filename, loadVector, loadRaster);
  }
}

ZCommand::ZCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    CtrlCutUndo(scene, parent), item(NULL), oldZ(0), newZ(0) {
  setText("ZCommand");
}

void ZCommand::undo() {
  if(item != NULL) {
    item->setZValue(oldZ);
  }
}

void ZCommand::redo() {
  if(item != NULL) {
    item->setZValue(newZ);
  }
}

LowerItemCommand::LowerItemCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    ZCommand(scene, parent) {
  setText("Lower item");
}

void LowerItemCommand::undo() {
  ZCommand::undo();
}

void LowerItemCommand::redo() {
  if(oldZ == std::numeric_limits<qreal>().max()) {
    QList<QGraphicsItem *> items = scene->selectedItems();
    qreal selZ = items[0]->zValue();
    oldZ = selZ;
    qreal z;
    qreal belowZ = selZ;

    foreach (QGraphicsItem *i, scene->items()) {
      z = i->zValue();
      if ((z > belowZ || belowZ == selZ) && z < selZ) {
        belowZ = z;
      }
    }
    items[0]->setZValue(belowZ-1);
    newZ = belowZ-1;
  } else {
    ZCommand::redo();
  }
}

RaiseItemCommand::RaiseItemCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    ZCommand(scene, parent) {
  setText("Raise item");
}

void RaiseItemCommand::undo() {
  ZCommand::undo();
}

void RaiseItemCommand::redo() {
  if(item == NULL) {
  QList<QGraphicsItem *> items = scene->selectedItems();
  item = items[0];
  qreal selZ = items[0]->zValue();
  oldZ = selZ;
  qreal z;
  qreal aboveZ = selZ;

  foreach (QGraphicsItem *i, scene->items()) {
    z = i->zValue();
    if ((z < aboveZ || aboveZ == selZ) && z > selZ) {
      aboveZ = z;
    }
  }

  newZ = aboveZ+1;
  items[0]->setZValue(aboveZ+1);
  } else
    ZCommand::redo();
}

LowerItemToBottomCommand::LowerItemToBottomCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    ZCommand(scene, parent) {
  setText("Lower item to bottom");
}

void LowerItemToBottomCommand::undo() {
  ZCommand::undo();
}

void LowerItemToBottomCommand::redo() {
  if(item == NULL) {
    QList<QGraphicsItem *> items = scene->selectedItems();
    item = items[0];
    qreal minz = items[0]->zValue();
    oldZ = minz;

    foreach (QGraphicsItem *i, scene->items()) {
      if (i->zValue() > -9999)
        minz = std::min(minz, i->zValue());
    }

    items[0]->setZValue(minz-1);
    newZ = minz-1;
  } else
    ZCommand::redo();
}

RaiseItemToTopCommand::RaiseItemToTopCommand(CtrlCutScene* scene, QUndoCommand *parent) :
    ZCommand(scene, parent) {
  setText("Raise item to top");
}

void RaiseItemToTopCommand::undo() {
  ZCommand::undo();
}

void RaiseItemToTopCommand::redo() {
  if(item == NULL) {
    QList<QGraphicsItem *> items = scene->selectedItems();
    item = items[0];
    qreal maxz = items[0]->zValue();
    oldZ = maxz;

    foreach (QGraphicsItem *i, scene->items()) {
      if (i->zValue() > -9999)
        maxz = std::max(maxz, i->zValue());
    }

    items[0]->setZValue(maxz+1);
    newZ = maxz+1;
  } else
    ZCommand::redo();
}


PasteCommand::PasteCommand(CtrlCutScene* scene, QUndoCommand *parent) : CtrlCutUndo(scene, parent) {
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

void MoveToOriginCommand::undo() {
  for(size_t i = 0; i < itemsMoved.size(); ++i) {
    itemsMoved[i]->setPos(oldPositions[i]);
  }
}

void MoveToOriginCommand::redo() {
  if(itemsMoved.empty()) {
    itemsMoved = this->scene->selectedItems();

    qreal minx = std::numeric_limits<qreal>().max();
    qreal miny = std::numeric_limits<qreal>().max();
    foreach (QGraphicsItem *item, itemsMoved) {
      const QPointF pos = item->pos();
      minx = std::min(pos.x(), minx);
      miny = std::min(pos.y(), miny);
    }

    foreach (QGraphicsItem *item, itemsMoved) {
      const QPointF pos = item->pos();
      oldPositions.append(pos);
      item->setPos(pos.x() - minx, pos.y() - miny);
      newPositions.append(item->pos());
    }
  } else {
    for(size_t i = 0; i < itemsMoved.size(); ++i) {
      itemsMoved[i]->setPos(newPositions[i]);
    }
  }
}

QString createCommandString(QGraphicsItem *item, const QPointF &pos) {
  return QObject::tr("Pos (%2, %3)").arg(pos.x()).arg(pos.y());
}
