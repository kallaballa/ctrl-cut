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
#include "CtrlCutScene.h"
#include "helpers/CutItem.h"
#include "event/CtrlCutEvent.h"
#include "Commands.h"
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include "helpers/DocumentHolder.h"
#include <Document.hpp>
#include <QGraphicsItem>

CtrlCutScene::CtrlCutScene(QObject *parent)
: QGraphicsScene(parent)
{
  this->docHolder.doc = new Document();
  using namespace Qt;

  QGraphicsPolygonItem *laserbed = new QGraphicsPolygonItem(QPolygonF(QRectF(QPointF(0,0), QSizeF(21600, 14400))));
  laserbed->setBrush(QBrush(Qt::white));
  laserbed->setZValue(-1000); // Render at the back
  this->addItem(laserbed);

  QPen gray(Qt::lightGray);
  for (int j=1000;j<14400;j+=1000) {
    this->addLine(0, j, 21600, j, gray);
  }
  for (int i=1000;i<21600;i+=1000) {
    this->addLine(i, 0, i, 14400, gray);
  }

/*
  typedef CtrlCutEvent CCE;
  typedef CCE::MoveItems MoveItems;
  typedef CCE::GroupItems GroupItems;
  typedef CCE::UngroupItems UngroupItems;

  qreal step = 1;
  CCE::registerAction(ControlModifier, Key_Up, MoveItems::UP(step));
  CCE::registerAction(ControlModifier, Key_Left, MoveItems::LEFT(step));
  CCE::registerAction(ControlModifier, Key_Down, MoveItems::DOWN(step));
  CCE::registerAction(ControlModifier, Key_Right, MoveItems::RIGHT(step));

  step = 10;
  CCE::registerAction(Key_Up, MoveItems::UP(step));
  CCE::registerAction(Key_Left, MoveItems::LEFT(step));
  CCE::registerAction(Key_Down, MoveItems::DOWN(step));
  CCE::registerAction(Key_Right, MoveItems::RIGHT(step));

  step = 100;
  CCE::registerAction(ShiftModifier, Key_Up, MoveItems::UP(step));
  CCE::registerAction(ShiftModifier, Key_Left, MoveItems::LEFT(step));
  CCE::registerAction(ShiftModifier, Key_Down, MoveItems::DOWN(step));
  CCE::registerAction(ShiftModifier, Key_Right, MoveItems::RIGHT(step));

  CCE::registerAction(ControlModifier, Key_G, GroupItems(*this));
  CCE::registerAction(ShiftModifier, Key_G, UngroupItems(*this));*/
}


/*
void CtrlCutScene::keyPressEvent(QKeyEvent *event) {
  CtrlCutEvent::act(*event, this->selectedItems());
}*/

void CtrlCutScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
   QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
                    event->buttonDownScenePos(Qt::LeftButton).y());
   std::cerr << "press: " << mousePos.x()<< ", " << mousePos.y() << std::endl;

   movingItem = itemAt(mousePos.x(), mousePos.y());
   foreach(QGraphicsItem* item, this->selectedItems()) {
     std::cerr << "item: " << item->pos().x()<< ", " << item->pos().y() << std::endl;
   }
   if (movingItem != 0 && event->button() == Qt::LeftButton) {
     std::cerr << "press: " << movingItem->pos().x()<< ", " << movingItem->pos().y() << std::endl;
     oldPos = movingItem->pos();
   }


   QGraphicsScene::mousePressEvent(event);
}

void CtrlCutScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
                   event->buttonDownScenePos(Qt::LeftButton).y());
  std::cerr << "release: " << mousePos.x()<< ", " << mousePos.y() << std::endl;

   if (movingItem != 0 && event->button() == Qt::LeftButton) {
     std::cerr << "release: " << movingItem->pos().x()<< ", " << movingItem->pos().y() << std::endl;
     if (oldPos != movingItem->pos())
           emit itemMoved(movingItem,oldPos);
       movingItem = 0;
   }
   QGraphicsScene::mouseReleaseEvent(event);
}

void CtrlCutScene::setDocumentHolder(DocumentHolder& docHolder) {
  this->reset();
  this->docHolder = docHolder;

  foreach(CutItem* ci, docHolder.cutItems) {
    this->addItem(ci);
  }

  foreach(EngraveItem* ei, docHolder.engraveItems) {
    this->addItem(ei);
  }
}

void CtrlCutScene::open(const QString& filename) {
  this->reset();
  load(filename);
}

void CtrlCutScene::load(const QString& filename) {
  if(!this->docHolder.doc)
    this->docHolder.doc = new Document();

  Document* doc = this->docHolder.doc;

  //FIXME make settings available through the gui
  doc->put(EngraveSettings::DITHERING, EngraveSettings::BAYER);
  doc->put(DocumentSettings::LOAD_ENGRAVING, true);
  doc->load(filename.toStdString());

  for (Document::CutIt it = doc->begin_cut(); it != doc->end_cut(); it++) {
    this->docHolder.cutItems.append(new CutItem(**it));
  }

  for (Document::EngraveIt it = doc->begin_engrave(); it != doc->end_engrave(); it++) {
    this->docHolder.engraveItems.append(new EngraveItem(**it));
  }
}

void CtrlCutScene::reset() {
  foreach(CutItem* ci, docHolder.cutItems) {
    this->removeItem(ci);
  }

  foreach(EngraveItem* ei, docHolder.engraveItems) {
    this->removeItem(ei);
  }

  this->docHolder.doc = NULL;

  while(!this->docHolder.cutItems.empty())
    this->docHolder.cutItems.takeFirst();

  while(!this->docHolder.engraveItems.empty())
    this->docHolder.engraveItems.takeFirst();
}

void CtrlCutScene::update() {
  foreach (QGraphicsItem *sitem, this->items()){
    AbstractCtrlCutItem* ccItem;
    if ((ccItem = dynamic_cast<AbstractCtrlCutItem*> (sitem->parentItem()))) {
      ccItem->commit();
    }
  }
  QGraphicsScene::update();
}

