#include "PathItem.hpp"
#include <QGraphicsPathItem>
#include <qpen.h>

PathItem::PathItem(const Path& path, QGraphicsItem *parent) :
    QGraphicsPathItem(parent), path(path) {
  QPainterPath painter;
  bool first=true;
  if (!path.empty()) {
    for (const Point& p : path) {
      if(first)
        painter.moveTo(p.x, p.y);
      else
        painter.lineTo(p.x, p.y);
      first = false;
    }

    QGraphicsPathItem::setPath(painter);
    QGraphicsPathItem::setPen(QPen(Qt::blue));
  }
}

