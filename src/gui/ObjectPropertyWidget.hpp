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
#ifndef _WIDGETDOCK_H_
#define _WIDGETDOCK_H_

#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"

#include <QDockWidget>

class ObjectPropertyWidget : public QWidget {
  Q_OBJECT
public:
  ObjectPropertyWidget(QWidget *parent = NULL);
  ~ObjectPropertyWidget();
  

  void applyLaserConfig(class Document &document);
  void updateLaserConfig(Document &document);

  void updateEngraveProperties(const EngraveSettings::KeyBase&  key);
  void updateCutProperties(const CutSettings::KeyBase&  key);
  void show(CutItem* ci);
  void show(EngraveItem* ei);
  void hide();

public slots:
  void on_power_update(const QString &);
  void on_speed_update(const QString &);
  void on_frequency_update(const QString &);
  void on_posX_update(const QString &);
  void on_posY_update(const QString &);
  void on_sort_update(int);
  void on_direction_update(int);
  void on_dithering_update(int);
private:
  enum State { NONE, Engraving, Cut};
  State current;

  CutItem* ci;
  EngraveItem* ei;
};

#endif
