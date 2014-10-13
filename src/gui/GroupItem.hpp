/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef GROUPITEM_H_
#define GROUPITEM_H_

#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>

class GroupItem : public QGraphicsItemGroup
{
public:
  GroupItem(QGraphicsItem *parent = 0) : QGraphicsItemGroup(parent) { }
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setPen(Qt::red);
    painter->drawRect(boundingRect());    
  }
  void addToGroup (QGraphicsItem *item) {
    qDebug() << "before width=" << QGraphicsItemGroup::boundingRect().width() << "height=" << QGraphicsItemGroup::boundingRect().height();
    QGraphicsItemGroup::addToGroup(item);
    qDebug() << "after  width=" << QGraphicsItemGroup::boundingRect().width() << "height=" << QGraphicsItemGroup::boundingRect().height();
  }
};

#endif
