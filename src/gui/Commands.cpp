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
#include <QtGui>
#include <Document.hpp>
#include <CtrlCutScene.hpp>
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include "Commands.hpp"
#include <cut/model/Reduce.hpp>

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
   std::cerr << "move undo" << std::endl;
     qraphicsItem->setPos(myOldPos);
     this->scene->update();
     setText(QObject::tr("Move %1")
         .arg(createCommandString(qraphicsItem, newPos)));
 }

 void MoveCommand::redo()
 {
   std::cerr << "move redo" << std::endl;
     qraphicsItem->setPos(newPos);
     this->scene->update();
     setText(QObject::tr("Move %1")
         .arg(createCommandString(qraphicsItem, newPos)));
 }

 DeleteCommand::DeleteCommand(CtrlCutScene* scene, QUndoCommand *parent) :
   QUndoCommand(parent), scene(scene) {
   QList<QGraphicsItem *> list = scene->selectedItems();
   list.first()->setSelected(false);
   graphicsItem = static_cast<QGraphicsItem *> (list.first());
   setText(QObject::tr("Delete %1")
     .arg(createCommandString(graphicsItem, graphicsItem->pos())));
 }

 void DeleteCommand::undo() {
   CutItem* ci;
   EngraveItem* ei;

   if((ci = dynamic_cast<CutItem* >(graphicsItem))) {
     scene->add(*ci);
   } else if((ei = dynamic_cast<EngraveItem* >(graphicsItem))) {
     scene->add(*ei);
   }

   scene->update();
 }

 void DeleteCommand::redo() {
   CutItem* ci;
   EngraveItem* ei;

   if((ci = dynamic_cast<CutItem* >(graphicsItem))) {
     scene->remove(*ci);
   } else if((ei = dynamic_cast<EngraveItem* >(graphicsItem))) {
     scene->remove(*ei);
   }
 }

 OpenCommand::OpenCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent) :
   QUndoCommand(parent), scene(scene), filename(filename) {
   if (!filename.isEmpty()) {
     this->oldDoc = this->scene->getDocumentHolder();
     setText("Open " + filename);
   }
 }

 void OpenCommand::undo() {
   this->scene->setDocumentHolder(this->oldDoc);
   this->scene->update();
 }

 void OpenCommand::redo() {
   if(!this->newDoc.doc) {
     this->scene->open(filename);
     this->newDoc = this->scene->getDocumentHolder();
   } else {
     this->scene->setDocumentHolder(this->newDoc);
   }

   this->scene->update();
 }

 ImportCommand::ImportCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent) :
   QUndoCommand(parent), scene(scene), filename(filename) {
   if (!filename.isEmpty()) {
     this->oldDoc = this->scene->getDocumentHolder();
     setText("Import " + filename);
   }
 }

 void ImportCommand::undo() {
   this->scene->setDocumentHolder(this->oldDoc);
   this->scene->update();
 }

 void ImportCommand::redo() {
   if(!this->newDoc.doc) {
     this->scene->load(filename);
     this->newDoc = this->scene->getDocumentHolder();
   } else {
     this->scene->setDocumentHolder(this->newDoc);
   }

   this->scene->update();
 }

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
       std::cerr << ci->cut.get(CutSettings::CPOS) << std::endl;
       Cut* newCut = new Cut(ci->cut.settings);
       std::cerr << newCut->get(CutSettings::CPOS) << std::endl;
       reduce(ci->cut, *newCut, Measurement(1, MM,75).in(PX));
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

QString createCommandString(QGraphicsItem *item, const QPointF &pos) {
  return QObject::tr("Pos (%2, %3)").arg(pos.x()).arg(pos.y());
}
