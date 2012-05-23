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
#ifndef CTRLCUTSCENE_H_
#define CTRLCUTSCENE_H_

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsItem>
#include "helpers/DocumentHolder.hpp"

class CtrlCutScene : public QGraphicsScene
{
  Q_OBJECT

public:

  CtrlCutScene(QObject *parent = 0);
  ~CtrlCutScene() {}

  void setDocumentHolder(DocumentHolder& doc);

  DocumentHolder getDocumentHolder() {
    return this->docHolder;
  }
  void add(class CutItem& cutItem);
  void remove(CutItem& cutItem);
  void add(class EngraveItem& engraveItem);
  void remove(EngraveItem& engraveItem);
  void open(const QString& filename);
  void load(const QString& filename);
  void reset();
  void update();
 // void keyPressEvent(QKeyEvent *event);

signals:
   void itemMoved(QGraphicsItem *movedItem, const QPointF &movedFromPosition);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  DocumentHolder docHolder;
  QGraphicsItemGroup *laserbed;
  QGraphicsItem *movingItem;
  QPointF oldPos;
};

#endif
