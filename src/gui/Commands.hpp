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

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <QUndoCommand>
#include <QGraphicsItem>
#include "helpers/DocumentHolder.hpp"

class Document;
class CtrlCutScene;

class MoveCommand: public QUndoCommand {
public:
  enum {
    Id = 1234
  };

  MoveCommand(CtrlCutScene* scene, QGraphicsItem *QGraphicsItem, const QPointF &oldPos,
      QUndoCommand *parent = NULL);

  void undo();
  void redo();
  //bool mergeWith(const QUndoCommand *command);
  int id() const {
    return Id;
  }

private:
  CtrlCutScene* scene;
  QGraphicsItem *qraphicsItem;
  QPointF myOldPos;
  QPointF newPos;
};

 class DeleteCommand : public QUndoCommand
 {
 public:
     DeleteCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);

     void undo();
     void redo();

private:
  CtrlCutScene* scene;
  QGraphicsItem *graphicsItem;
};
class OpenCommand: public QUndoCommand {
public:
  OpenCommand(CtrlCutScene* scene, const QString &filename,
      QUndoCommand *parent = NULL);

  void undo();
  void redo();

private:
  CtrlCutScene* scene;
  QString filename;
  DocumentHolder oldDoc;
  DocumentHolder newDoc;
};

class NewCommand: public QUndoCommand {
public:
  NewCommand(CtrlCutScene* scene,
      QUndoCommand *parent = NULL);

  void undo();
  void redo();

private:
  CtrlCutScene* scene;
  DocumentHolder oldDoc;
  DocumentHolder newDoc;
};

class ImportCommand: public QUndoCommand {
public:
  ImportCommand(CtrlCutScene* scene, const QString &filename, QUndoCommand *parent = NULL);

  void undo();
  void redo();

private:
  CtrlCutScene* scene;
  QString filename;
  DocumentHolder oldDoc;
  DocumentHolder newDoc;
};

class SimplifyCommand: public QUndoCommand {
public:
  SimplifyCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();

  CtrlCutScene* scene;
  QList<CutItem* > oldCutItems;
  QList<CutItem* > newCutItems;
};

QString createCommandString(QGraphicsItem *item, const QPointF &point);


#endif /* COMMANDS_H_ */
