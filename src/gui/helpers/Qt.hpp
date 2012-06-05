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

#include <qgraphicsitem.h>
#include <qimage.h>
#include "engrave/image/Image.hpp"
#include <qgraphicseffect.h>
#include <qstyleoption.h>
#include <qpainter.h>

#ifndef QTMAKE_H_
#define QTMAKE_H_

class QtMake {
public:
  static QImage& make_QImage(AbstractImage& image) {
    QImage *img;

    if (dynamic_cast<BitmapImage*>(&image)) {
      img = new QImage((const uchar *)image.data(), image.width(), image.height(),
                        image.rowstride(), QImage::Format_Mono);
      QVector<QRgb> colortable;
      colortable.append(qRgba(0,0,0,255));
      colortable.append(qRgba(0,0,0,0));
      img->setColorTable(colortable);
    }
     else if (dynamic_cast<GrayscaleImage*>(&image)) {
      img = new QImage((const uchar *)image.data(), image.width(), image.height(),
                        image.rowstride(), QImage::Format_Indexed8);
      QVector<QRgb> colortable;
      for(int i=0;i<255;i++) colortable.append(qRgba(i,i,i,255));
      colortable.append(qRgba(255,255,255,0));
      img->setColorTable(colortable);
    }

    return *img;
  }
};


class AbstractCtrlCutItem : public QGraphicsItemGroup {
public:
  virtual void commit() = 0;

  void setHighlighted(bool enabled) {
    return;
    // causes performance issues
    QGraphicsColorizeEffect* highlight = new QGraphicsColorizeEffect();
    highlight->setColor(Qt::green);
    if(enabled) {
      if(QGraphicsItemGroup::graphicsEffect() == NULL)
        QGraphicsItemGroup::setGraphicsEffect(highlight);
    }
    else {
      if(QGraphicsItemGroup::graphicsEffect() != NULL)
        QGraphicsItemGroup::setGraphicsEffect(NULL);
    }
  }
};

#endif /* QTMAKE_H_ */
