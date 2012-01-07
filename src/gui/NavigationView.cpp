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
 
  // this->ensureVisible(laserbed);

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

void NavigationView::updateSceneRect(const QRectF &rect)
{
  setSceneRect(rect.adjusted(-rect.width(), -rect.height(), rect.width(), rect.height()));
}

