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
