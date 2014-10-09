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


class CtrlCutUndo: public QUndoCommand {
public:
  CtrlCutUndo(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  virtual ~CtrlCutUndo(){};

  CtrlCutScene* scene;
private:
};

class MoveCommand: public CtrlCutUndo {
public:
  MoveCommand(CtrlCutScene* scene, QGraphicsItem *QGraphicsItem, const QPointF &oldPos,
      QUndoCommand *parent = NULL);

  void undo();
  void redo();
  //bool mergeWith(const QUndoCommand *command);

private:
  QGraphicsItem *qraphicsItem;
  QPointF myOldPos;
  QPointF newPos;
};

class GroupCommand : public CtrlCutUndo
{
public:
  GroupCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  virtual void redo();
  virtual void undo();
  static class CtrlCutGroupItem *groupItems(CtrlCutScene* scene, QList<QGraphicsItem*> items);
private:
  class CtrlCutGroupItem *group;
};

class UnGroupCommand : public CtrlCutUndo
{
public:
  UnGroupCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  virtual void redo();
  virtual void undo();
  static QList<QGraphicsItem*> ungroup(CtrlCutScene* scene, CtrlCutGroupItem *group);
private:
  QList<QGraphicsItem *> items;
};

class DeleteCommand : public CtrlCutUndo
{
public:
    DeleteCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
    void undo();
    void redo();
private:
    AbstractCtrlCutItem* deleted;
};

class OpenCommand: public CtrlCutUndo {
public:
  OpenCommand(CtrlCutScene* scene, const QString &filename,
      QUndoCommand *parent = NULL);

  void undo();
  void redo();
private:
  QString filename;
};

class SaveCommand: public CtrlCutUndo {
public:
  SaveCommand(CtrlCutScene* scene, const QString& filename, QUndoCommand *parent = NULL);

  void undo();
  void redo();

  const QString filename;
};

class ImportCommand: public CtrlCutUndo {
public:
  ImportCommand(CtrlCutScene* scene, const QString &filename, QUndoCommand *parent = NULL);

  void undo();
  void redo();

private:
  std::vector<AbstractCtrlCutItem*> imported;
  QString filename;
};

class SimplifyCommand: public QUndoCommand {
public:
  SimplifyCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();

  CtrlCutScene* scene;
  QList<class CutItem* > oldCutItems;
  QList<CutItem* > newCutItems;
};

class ZCommand: public CtrlCutUndo {
public:
  ZCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
  QGraphicsItem* item;
  qreal oldZ;
  qreal newZ;
};

class LowerItemCommand: public ZCommand {
public:
  LowerItemCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class RaiseItemCommand: public ZCommand {
public:
  RaiseItemCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class LowerItemToBottomCommand: public ZCommand {
public:
  LowerItemToBottomCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class RaiseItemToTopCommand: public ZCommand {
public:
  RaiseItemToTopCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class PasteCommand: public CtrlCutUndo {
  QList<class AbstractCtrlCutItem *> itemsAdded;
public:
  PasteCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
};

class MoveToOriginCommand: public CtrlCutUndo {
public:
  MoveToOriginCommand(CtrlCutScene* scene, QUndoCommand *parent = NULL);
  void undo();
  void redo();
  QList<QGraphicsItem *> itemsMoved;
  QList<QPointF> oldPositions;
  QList<QPointF> newPositions;
};

QString createCommandString(QGraphicsItem *item, const QPointF &point);


#endif /* COMMANDS_H_ */
