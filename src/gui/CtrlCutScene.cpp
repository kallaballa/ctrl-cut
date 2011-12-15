#include "CtrlCutScene.h"
#include <QGraphicsItem>
#include <QKeyEvent>

CtrlCutScene::CtrlCutScene(QObject *parent) : QGraphicsScene(parent)
{
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

}

void CtrlCutScene::keyPressEvent(QKeyEvent *event)
{
  QPointF delta;

  switch (event->key()) {
  case Qt::Key_Up:
    delta.setY(-10);
    break;
  case Qt::Key_Down:
    delta.setY(10);
    break;
  case Qt::Key_Left:
    delta.setX(-10);
    break;
  case Qt::Key_Right:
    delta.setX(10);
    break;
  }

  if (event->modifiers() & Qt::ShiftModifier) delta *= 10;

  foreach (QGraphicsItem *item, this->selectedItems()) {
    QPointF pos = item->pos();
    pos += delta;
    item->setPos(pos);
  }
}

