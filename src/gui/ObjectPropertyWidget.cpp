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

#include "ObjectPropertyWidget.hpp"
#include "MainWindow.hpp"
#include "config/EngraveSettings.hpp"
#include "config/CutSettings.hpp"
#include "config/DocumentSettings.hpp"
#include <assert.h>

typedef DocumentSettings DS;
typedef EngraveSettings ES;
typedef CutSettings CS;

ObjectPropertyWidget::ObjectPropertyWidget(QWidget *parent) : QWidget(parent), current(NONE) {
  // setupUi(this);
}

ObjectPropertyWidget::~ObjectPropertyWidget() {
}

void ObjectPropertyWidget::updateEngraveProperties(const EngraveSettings::KeyBase&  key) {

}

void ObjectPropertyWidget::updateCutProperties(const CutSettings::KeyBase&  key) {
  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget());

}

void ObjectPropertyWidget::show(CutItem* ci) {
  this->ci = ci;
  this->ci->cut.settings.setUpdateTrigger(bind(&ObjectPropertyWidget::updateCutProperties, this, _1));

  Point pos = this->ci->cut.get(CS::CPOS);
  int speed = this->ci->cut.get(CS::CSPEED);
  int power = this->ci->cut.get(CS::CPOWER);
  int freq = this->ci->cut.get(CS::FREQUENCY);
  CS::Optimize sort = this->ci->cut.get(CS::OPTIMIZE);
  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());
  mainw->posX->setText(QString::number(pos.x));
  mainw->posY->setText(QString::number(pos.y));
  mainw->speed->setValue(speed);
  mainw->power->setValue(power);
  mainw->frequency->setValue(freq);

  mainw->frequency->show();
  mainw->freqLabel->show();
  mainw->sort->show();
  mainw->sortLabel->show();

  mainw->ditherLabel->hide();
  mainw->direction->hide();
  mainw->directionLabel->hide();
  mainw->dithering->hide();
  mainw->ditherLabel->hide();
  mainw->direction->hide();
  mainw->directionLabel->hide();

  QString strSort;
  switch (sort) {
  case CS::SIMPLE:
    strSort = "Simple";
    break;
  case CS::INNER_OUTER:
    strSort = "Inner-Outer";
    break;
  case CS::SHORTEST_PATH:
    strSort = "Shortest Path";
    break;
  default:
    assert(false && "Unhandled cut optimize setting");
    break;
  }

  mainw->sort->setCurrentIndex(mainw->sort->findText(strSort));
  QWidget::setEnabled(true);
  this->current = Cut;
}

void ObjectPropertyWidget::show(EngraveItem* ei) {
  this->ei = ei;
  this->ei->engraving.settings.setUpdateTrigger(bind(&ObjectPropertyWidget::updateEngraveProperties, this, _1));

  Point pos = this->ei->engraving.get(ES::EPOS);
  int speed = this->ei->engraving.get(ES::ESPEED);
  int power = this->ei->engraving.get(ES::EPOWER);

  ES::Dithering dither = this->ei->engraving.get(ES::DITHERING);
  ES::Direction direction = this->ei->engraving.get(ES::DIRECTION);

  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());


  mainw->frequency->hide();
  mainw->freqLabel->hide();
  mainw->sort->hide();
  mainw->sortLabel->hide();

  mainw->ditherLabel->show();
  mainw->direction->show();
  mainw->directionLabel->show();
  mainw->dithering->show();
  mainw->ditherLabel->show();
  mainw->direction->show();
  mainw->directionLabel->show();

  mainw->posX->setText(QString::number(pos.x));
  mainw->posY->setText(QString::number(pos.y));
  mainw->speed->setValue(speed);
  mainw->power->setValue(power);

  QString ditherstr;
  switch (dither) {
  case EngraveSettings::BAYER:
    ditherstr = "Bayer";
    break;
  case EngraveSettings::FLOYD_STEINBERG:
    ditherstr = "Floyd-Steinberg";
    break;
  case EngraveSettings::JARVIS:
    ditherstr = "Jarvis";
    break;
  case EngraveSettings::BURKE:
    ditherstr = "Burke";
    break;
  case EngraveSettings::STUCKI:
    ditherstr = "Stucki";
    break;
  case EngraveSettings::SIERRA2:
    ditherstr = "Sierra2";
    break;
  case EngraveSettings::SIERRA3:
    ditherstr = "Sierra3";
    break;
  case EngraveSettings::DEFAULT_DITHERING:
    ditherstr = "Default";
    break;
  default:
    assert(false && "Unhandled raster dithering setting");
    break;
  }

  mainw->dithering->setCurrentIndex(mainw->dithering->findText(ditherstr));
  mainw->direction->setCurrentIndex(direction);
  QWidget::setEnabled(true);
  this->current = Engraving;
}

void ObjectPropertyWidget::hide() {
  this->ei = ei;
  if(this->current == Cut) {
    this->ci->cut.settings.deleteUpdateTrigger();
    this->ci = NULL;
  } else if(this->current == Engraving) {
    this->ei->engraving.settings.deleteUpdateTrigger();
    this->ei = NULL;
  }
  QWidget::setEnabled(false);
  this->current = NONE;
}

void ObjectPropertyWidget::on_power_update(const QString& p) {
  if(p.length() > 0) {
    QString val = QString(p).remove(p.length() -1,1);
    std::cerr << val.toStdString() << std::endl;
    if(current == Cut)
      this->ci->cut.put(CS::CPOWER, val.toInt());
    else if(current == Engraving) {
      this->ei->engraving.put(ES::EPOWER, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_speed_update(const QString& s)  {
  if(s.length() > 0) {
    QString val = QString(s).remove(s.length() -1,1);
    std::cerr << val.toStdString() << std::endl;
    if(current == Cut)
      this->ci->cut.put(CS::CSPEED, val.toInt());
    else if(current == Engraving) {
      this->ei->engraving.put(ES::ESPEED, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_frequency_update(const QString& f)  {
  if(f.length() > 0 && current == Cut) {
    QString val = QString(f).remove(f.length() -2,2);
    std::cerr << val.toStdString() << std::endl;
    this->ci->cut.put(CS::FREQUENCY, val.toInt());
  }
}

void ObjectPropertyWidget::on_posX_update(const QString& x)  {
  if(current == Cut) {
    Point pos = this->ci->cut.get(CS::CPOS);
    pos.x = x.toInt();
    this->ci->cut.put(CS::CPOS, pos);
    this->ci->setPos(pos.x, pos.y);
  }
  else if(current == Engraving) {
    Point pos = this->ei->engraving.get(ES::EPOS);
    pos.x = x.toInt();
    this->ei->engraving.put(ES::EPOS, pos);
    this->ei->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_posY_update(const QString& y)  {
  if(current == Cut) {
    Point pos = this->ci->cut.get(CS::CPOS);
    pos.y = y.toInt();
    this->ci->cut.put(CS::CPOS, pos);
    this->ci->setPos(pos.x, pos.y);
  }
  else if(current == Engraving) {
    Point pos = this->ei->engraving.get(ES::EPOS);
    pos.y = y.toInt();
    this->ei->engraving.put(ES::EPOS, pos);
    this->ei->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_sort_update(int s) {
  if(current == Cut) {
    switch(s) {
    case 0:
      this->ci->cut.put(CS::OPTIMIZE, CS::SIMPLE);
        break;
    case 1:
      this->ci->cut.put(CS::OPTIMIZE, CS::INNER_OUTER);
        break;
    case 2:
      this->ci->cut.put(CS::OPTIMIZE, CS::SHORTEST_PATH);
        break;
    }
  }
}

void ObjectPropertyWidget::on_direction_update(int d) {
  if(current == Engraving) {
      if(d == 0)
        this->ei->engraving.put(ES::DIRECTION, ES::TOPDOWN);
      else
        this->ei->engraving.put(ES::DIRECTION, ES::BOTTOMUP);
  }
}

void ObjectPropertyWidget::on_dithering_update(int d) {
  if(current == Engraving) {
    switch(d) {
    case 0:
      this->ei->engraving.put(ES::DITHERING, ES::DEFAULT_DITHERING);
        break;
    case 1:
      this->ei->engraving.put(ES::DITHERING, ES::BAYER);
        break;
    case 2:
      this->ei->engraving.put(ES::DITHERING, ES::FLOYD_STEINBERG);
        break;
    case 3:
      this->ei->engraving.put(ES::DITHERING, ES::JARVIS);
        break;
    case 4:
      this->ei->engraving.put(ES::DITHERING, ES::STUCKI);
        break;
    case 5:
      this->ei->engraving.put(ES::DITHERING, ES::BURKE);
        break;
    case 6:
      this->ei->engraving.put(ES::DITHERING, ES::SIERRA2);
        break;
    case 7:
      this->ei->engraving.put(ES::DITHERING, ES::SIERRA3);
        break;
    }
  }
}
