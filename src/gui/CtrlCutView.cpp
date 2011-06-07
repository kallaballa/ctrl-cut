#include "CtrlCutView.h"
 
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <math.h>
#include <iostream>
 
CtrlCutView::CtrlCutView(QWidget* parent) : QGraphicsView(parent)
{
  setAlignment(0);
  // setTransformationAnchor(QGraphicsView::NoAnchor);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
 
  QGraphicsScene *scene = new QGraphicsScene(this);
  this->setScene(scene);

  // FIXME: Use device's size from LaserConfig
  QGraphicsPolygonItem *laserbed = new QGraphicsPolygonItem(QPolygonF(QRectF(QPointF(0,0), QSizeF(21600, 14400))));
  laserbed->setZValue(-1000); // Render at the back
  scene->addItem(laserbed);

  this->ensureVisible(laserbed);

  //Set-up the view
  QRectF rect = scene->sceneRect();
  rect.adjust(-5000, -5000, 5000, 5000);
  setSceneRect(rect);

  setCursor(Qt::OpenHandCursor);
}
 
void CtrlCutView::wheelEvent(QWheelEvent* event)
{
  scaleView(pow(2.0, event->delta() / 240.0));
  //  centerOn(mapToScene(event->pos()));
}

void CtrlCutView::scaleView(double scaleFactor)
{
  double factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
  if (factor < 0.01 || factor > 1000) return;
  
  scale(scaleFactor, scaleFactor);
}

void CtrlCutView::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
  case Qt::Key_Up:
    translate(0, 100);
    break;
  case Qt::Key_Down:
    translate(0, -100);
    break;
  case Qt::Key_Left:
    translate(100, 0);
    break;
  case Qt::Key_Right:
    translate(-100, 0);
    break;
  case Qt::Key_Plus:
    scaleView(1.2);
    break;
  case Qt::Key_Minus:
    scaleView(1 / 1.2);
    break;
  default:
    QGraphicsView::keyPressEvent(event);
  }
}

void CtrlCutView::mouseDoubleClickEvent(QMouseEvent *event)
{
  // centerOn(mapToScene(event->pos()));
  // translate(100,100);

  QTransform trans = transform();
  std::cout
    << trans.m11() << " "
    << trans.m12() << " "
    << trans.m13() << "\n"
    << trans.m21() << " "
    << trans.m22() << " "
    << trans.m23() << "\n"
    << trans.m31() << " "
    << trans.m32() << " "
    << trans.m33() << "\n";
}

