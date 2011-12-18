#include "CtrlCutScene.h"
#include "event/CtrlCutEvent.h"
#include <QGraphicsItem>
#include <QKeyEvent>

CtrlCutScene::CtrlCutScene(QObject *parent) : QGraphicsScene(parent)
{
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
  CCE::registerAction(ShiftModifier, Key_G, UngroupItems(*this));
}

void CtrlCutScene::keyPressEvent(QKeyEvent *event) {
  CtrlCutEvent::act(*event, this->selectedItems());
}
