/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef NAVIGATIONVIEW_H_
#define NAVIGATIONVIEW_H_
 
#include <QGraphicsView>
#include <QGraphicsRectItem>
 
class NavigationView : public QGraphicsView
{
  Q_OBJECT
public:
  NavigationView(QWidget* parent = NULL);

public slots:
  void updateSceneRect(const QRectF &);

signals:
  void matrixUpdated();

private:
  void scaleView(qreal scaleFactor, QPointF center);
  
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);
  void wheelEvent(QWheelEvent* event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
};
 
#endif
