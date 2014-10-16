/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include <qimage.h>
#include "engrave/image/Image.hpp"
#include <qstyleoption.h>
#include <string>

#ifndef QTMAKE_H_
#define QTMAKE_H_

void showErrorDialog(std::string text, std::string detail);
void showWarningDialog(std::string text, std::string detail);
int showYesNoDialog(std::string title, std::string text, std::string detail);


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
    else assert(false);

    return *img;
  }
};

#endif /* QTMAKE_H_ */
