/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Commands.hpp"
#include "CtrlCutScene.hpp"
#include "ImportDialog.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include <Document.hpp>
#include <cut/operations/Reduce.hpp>
#include "cut/operations/Merge.hpp"
#include "MainWindow.hpp"

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
    bool loadVector = true;
    bool loadRaster = false;
    imported = this->scene->load(filename, loadVector, loadRaster);
    foreach(QGraphicsItem* item, this->scene->items()) {
      item->setSelected(false);
    }

    foreach(QGraphicsItem* item, imported) {
      item->setSelected(true);
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
    CutItem* ci;
    EngraveItem* ei;

    std::vector<Box> boxes;
    foreach (AbstractCtrlCutItem *item, this->scene->itemClipboard) {
      itemsAdded.append(item->clone());
      if((ci = dynamic_cast<CutItem* >(item))) {
        boxes.push_back(find_bounding_box(*ci->cut.get()));
      } else if((ei = dynamic_cast<EngraveItem* >(item))) {
        boxes.push_back(find_bounding_box(*ei->engraving.get()));
      }
    }

    //move items to current mouse location
    Box itemsAddedBox = find_bounding_box(boxes);
    Point mousePos(this->scene->mousePos.x(), this->scene->mousePos.y());
    Point center(itemsAddedBox.min_corner.x + (itemsAddedBox.width() / 2), itemsAddedBox.min_corner.y + (itemsAddedBox.height() / 2));

    foreach (AbstractCtrlCutItem *item, itemsAdded) {
      QPointF itemPos = item->pos();
      QPointF newPos = QPointF(
          itemPos.x() + mousePos.x - itemsAddedBox.min_corner.x - (itemsAddedBox.width() / 2),
          itemPos.y() + mousePos.y - itemsAddedBox.min_corner.y - (itemsAddedBox.height() / 2)
      );
      item->setPos(newPos);
      item->commit();
    }
    this->scene->update();
  }

  foreach (AbstractCtrlCutItem *item, itemsAdded) {
    this->scene->add(*item);
  }
}

PasteSettingsCommand::PasteSettingsCommand(CtrlCutScene* scene, QUndoCommand *parent) : CtrlCutUndo(scene, parent) {
  setText("Paste settings");
}

void PasteSettingsCommand::undo() {
  CutItem* ci;
  EngraveItem* ei;
  QGraphicsItem *item;
  for(size_t i = 0; i < itemsChanged.size(); ++i) {
    item = itemsChanged[i];

    if((ci = dynamic_cast<CutItem* >(item))) {
      ci->cut->settings = *static_cast<CutSettings*>(&oldSettings[i]);
    } else if((ei = dynamic_cast<EngraveItem* >(item))) {
      ei->engraving->settings = *static_cast<EngraveSettings*>(&oldSettings[i]);
    }
  }
  MainWindow* mainw = qobject_cast<MainWindow*>(this->scene->parent());
  mainw->objectProperties->update();
}
void PasteSettingsCommand::redo() {
  CutItem* ci;
  CutSettings* cs;
  EngraveItem* ei;
  EngraveSettings* es;

  if(itemsChanged.empty()) {
    newSetting = *this->scene->settingsClipboard[0];

    foreach(QGraphicsItem *item, this->scene->selectedItems()) {
      if((ci = dynamic_cast<CutItem* >(item))) {
        itemsChanged.append(ci);
        oldSettings.append(ci->cut->settings);
        if((cs = static_cast<CutSettings*>(&newSetting))) {
          ci->cut->settings.copy(*cs);
        }
      } else if((ei = dynamic_cast<EngraveItem* >(item))) {
        itemsChanged.append(ei);
        oldSettings.append(ei->engraving->settings);
        if((es = static_cast<EngraveSettings*>(&newSetting))) {
          ei->engraving->settings.copy(*es);
        }
      }
    }
  } else {
    QGraphicsItem *item;
    for(size_t i = 0; i < itemsChanged.size(); ++i) {
      item = itemsChanged[i];

      if((ci = dynamic_cast<CutItem* >(item))) {
        ci->cut->settings.copy(*static_cast<CutSettings*>(&newSetting));
      } else if((ei = dynamic_cast<EngraveItem* >(item))) {
        ei->engraving->settings.copy(*static_cast<EngraveSettings*>(&newSetting));
      }
    }
  }
  MainWindow* mainw = qobject_cast<MainWindow*>(this->scene->parent());
  mainw->objectProperties->update();
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

    qreal maxx = std::numeric_limits<qreal>().min();
    qreal maxrb = std::numeric_limits<qreal>().min();
    qreal miny = std::numeric_limits<qreal>().max();
    foreach (QGraphicsItem *item, itemsMoved) {
      const QPointF pos = item->pos();
      maxrb = std::max(item->boundingRect().right(), maxrb);
      maxx = std::max(pos.x(), maxx);
      miny = std::min(pos.y(), miny);
    }

    AbstractCtrlCutItem* acci;
    foreach (QGraphicsItem *item, itemsMoved) {
      const QPointF pos = item->pos();
      oldPositions.append(pos);
      item->setPos((this->scene->width() - maxrb), pos.y() - miny);
      newPositions.append(item->pos());
      if((acci = dynamic_cast<AbstractCtrlCutItem*>(item))) {
        acci->commit();
      }
    }
  } else {
    AbstractCtrlCutItem* acci;
    for(size_t i = 0; i < itemsMoved.size(); ++i) {
      itemsMoved[i]->setPos(newPositions[i]);
      if((acci = dynamic_cast<AbstractCtrlCutItem*>(itemsMoved[i]))) {
        acci->commit();
      }
    }
  }
}

MergeCommand::MergeCommand(CtrlCutScene* scene, QUndoCommand *parent) : CtrlCutUndo(scene, parent), itemGenerated(NULL) {
  setText("Merge");
}

void MergeCommand::undo() {
  foreach(AbstractCtrlCutItem* item, this->itemsRemoved) {
    this->scene->add(*item);
  }
  this->scene->remove(*itemGenerated);
}

void MergeCommand::redo() {
  if(itemGenerated == NULL) {
    if(this->scene->selectedItems().size() < 1)
      return;

    bool first = true;
    CutPtr merged = this->scene->getDocumentHolder()->doc->newCut();
    foreach(QGraphicsItem* item, this->scene->selectedItems()) {
      CutItem* ci = dynamic_cast<CutItem*>(item);
      assert(ci);
      if(first)
        merged->settings = ci->cut->settings;

      merge(*ci->cut.get(), *merged.get());
      this->scene->remove(*ci);
      itemsRemoved.append(ci);
      first = false;
    }

    itemGenerated = new CutItem(merged);
    this->scene->addItem(itemGenerated);
    this->scene->getDocumentHolder()->addItem(*itemGenerated);
  } else {
    foreach(AbstractCtrlCutItem* item, this->itemsRemoved) {
      this->scene->remove(*item);
    }
    this->scene->add(*itemGenerated);
  }
}

QString createCommandString(QGraphicsItem *item, const QPointF &pos) {
  return QObject::tr("Pos (%2, %3)").arg(pos.x()).arg(pos.y());
}
