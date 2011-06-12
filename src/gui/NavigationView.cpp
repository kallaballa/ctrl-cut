#include "NavigationView.h"
 
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <math.h>
#include <iostream>
 
NavigationView::NavigationView(QWidget* parent) : QGraphicsView(parent)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setAlignment(0);
  setTransformationAnchor(QGraphicsView::NoAnchor);
  //  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  setDragMode(QGraphicsView::ScrollHandDrag);

  setBackgroundBrush(Qt::lightGray);
 
  QGraphicsScene *scene = new QGraphicsScene(this);
  this->setScene(scene);

  QGraphicsPolygonItem *laserbed = new QGraphicsPolygonItem(QPolygonF(QRectF(QPointF(0,0), QSizeF(21600, 14400))));
  laserbed->setBrush(QBrush(Qt::white));
  laserbed->setZValue(-1000); // Render at the back
  scene->addItem(laserbed);

  for (int j=1000;j<14400;j+=1000) {
    scene->addLine(0, j, 21600, j);
  }
  for (int i=1000;i<21600;i+=1000) {
    scene->addLine(i, 0, i, 14400);
  }

  this->ensureVisible(laserbed);

  //Set-up the view
  QRectF rect = scene->sceneRect();
  rect.adjust(-rect.width(), -rect.height(), rect.width(), rect.height());
  setSceneRect(rect);

  setCursor(Qt::OpenHandCursor);
}
 
void NavigationView::wheelEvent(QWheelEvent* event)
{
  scaleView(pow(2.0, event->delta() / 240.0), mapToScene(event->pos()));
}

void NavigationView::scaleView(double scaleFactor, QPointF center)
{
  double factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
  if (factor < 0.01 || factor > 1000) return;

  // Before we scale, the point under the mouse is center
  scale(scaleFactor, scaleFactor);

  // Where is the mouse now?
  QPointF newp = mapToScene(this->viewport()->mapFromGlobal(QCursor::pos()));

  QPointF diff = newp - center;
  translate(diff.x(), diff.y());
}

void NavigationView::keyPressEvent(QKeyEvent *event)
{
  QRectF g = this->mapToScene(0, 0, this->viewport()->geometry().width(), this->viewport()->geometry().height()).boundingRect();

  switch (event->key()) {
  case Qt::Key_Up:
    translate(0, g.width()/25);
    break;
  case Qt::Key_Down:
    translate(0, -g.width()/25);
    break;
  case Qt::Key_Left:
    translate(g.height()/25, 0);
    break;
  case Qt::Key_Right:
    translate(-g.height()/25, 0);
    break;
  case Qt::Key_Plus:
    scaleView(1.2, mapToScene(this->mapFromGlobal(QCursor::pos())));
    break;
  case Qt::Key_Minus:
    scaleView(1 / 1.2, mapToScene(this->mapFromGlobal(QCursor::pos())));
    break;
  default:
    QGraphicsView::keyPressEvent(event);
  }
}

void NavigationView::mouseDoubleClickEvent(QMouseEvent *event)
{
  // centerOn(mapToScene(event->pos()));
  // translate(100,100);

  QPointF p = mapToScene(event->pos());
  printf("%f %f\n", p.x(), p.y());
}

void NavigationView::paintEvent(QPaintEvent *event)
{
  QGraphicsView::paintEvent(event);
  emit matrixUpdated();
}

