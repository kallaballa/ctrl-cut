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
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include <QUndoView>
#include <assert.h>

typedef DocumentSettings DS;
typedef EngraveSettings ES;
typedef CutSettings CS;

ObjectPropertyWidget::ObjectPropertyWidget(QWidget *parent) : QWidget(parent), currentState(NONE), currentUnit(MM),currentResolution(600) {
  disable();
}

ObjectPropertyWidget::~ObjectPropertyWidget() {
}

void ObjectPropertyWidget::updateEngraveProperties(const EngraveSettings::KeyBase&  key) {
  this->enable(this->ei);
}

void ObjectPropertyWidget::updateCutProperties(const CutSettings::KeyBase&  key) {
  this->enable(this->ci);
}

void ObjectPropertyWidget::enable(AbstractCtrlCutItem* item) {
  if (EngraveItem *ei = dynamic_cast<EngraveItem*>(item)) {
    enableEngraveItem(ei);
  }
  else if (CutItem *ci = dynamic_cast<CutItem*>(item)) {
    enableCutItem(ci);
  }
  // FIXME: Enable group, potentially mixed
}

void ObjectPropertyWidget::enableCutItem(CutItem* ci) {
  this->disable();
  this->ci = ci;
  this->ci->cut.settings.setUpdateTrigger(bind(&ObjectPropertyWidget::updateCutProperties, this, _1));

  Point pos = this->ci->cut.get(CS::CPOS);
  int speed = this->ci->cut.get(CS::CSPEED);
  int power = this->ci->cut.get(CS::CPOWER);
  int freq = this->ci->cut.get(CS::FREQUENCY);
  CS::Sort sort = this->ci->cut.get(CS::SORT);
  this->currentResolution = this->ci->cut.get(DS::RESOLUTION);

  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());
  double posX = Distance(pos.x,PX,this->currentResolution).in(this->currentUnit);
  double posY = Distance(pos.y,PX,this->currentResolution).in(this->currentUnit);

  mainw->posX->setText(QString::number(posX));
  mainw->posY->setText(QString::number(posY));
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
  this->currentState = Cut;
}

void ObjectPropertyWidget::enableEngraveItem(EngraveItem* ei) {
  this->disable();
  this->ei = ei;
  this->ei->engraving.settings.setUpdateTrigger(bind(&ObjectPropertyWidget::updateEngraveProperties, this, _1));

  Point pos = this->ei->engraving.get(ES::EPOS);
  int speed = this->ei->engraving.get(ES::ESPEED);
  int power = this->ei->engraving.get(ES::EPOWER);
  this->currentResolution = this->ei->engraving.get(DS::RESOLUTION);
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

  double posX = Distance(pos.x,PX,this->currentResolution).in(this->currentUnit);
  double posY = Distance(pos.y,PX,this->currentResolution).in(this->currentUnit);

  mainw->posX->setText(QString::number(posX));
  mainw->posY->setText(QString::number(posY));
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
  this->currentState = Engraving;
}

void ObjectPropertyWidget::update() {
  if(this->ci != NULL)
    this->enable(this->ci);
  if(this->ei != NULL)
    this->enable(this->ei);
}

void ObjectPropertyWidget::disable() {
  if(this->currentState == Cut) {
    this->ci->cut.settings.deleteUpdateTrigger();
    this->ci = NULL;
  } else if(this->currentState == Engraving) {
    this->ei->engraving.settings.deleteUpdateTrigger();
    this->ei = NULL;
  }
  QWidget::setEnabled(false);
  this->currentState = NONE;
}

void ObjectPropertyWidget::on_power_update(const QString& p) {
  if(p.length() > 0) {
    QString val = QString(p).remove(p.length() -1,1);
    std::cerr << val.toStdString() << std::endl;
    if(currentState == Cut)
      this->ci->cut.put(CS::CPOWER, val.toInt());
    else if(currentState == Engraving) {
      this->ei->engraving.put(ES::EPOWER, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_speed_update(const QString& s)  {
  if(s.length() > 0) {
    QString val = QString(s).remove(s.length() -1,1);
    std::cerr << val.toStdString() << std::endl;
    if(currentState == Cut)
      this->ci->cut.put(CS::CSPEED, val.toInt());
    else if(currentState == Engraving) {
      this->ei->engraving.put(ES::ESPEED, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_frequency_update(const QString& f)  {
  if(f.length() > 0 && currentState == Cut) {
    QString val = QString(f).remove(f.length() -2,2);
    std::cerr << val.toStdString() << std::endl;
    this->ci->cut.put(CS::FREQUENCY, val.toInt());
  }
}

void ObjectPropertyWidget::on_posX_update(const QString& x)  {
  if(currentState == Cut) {
    Point pos = this->ci->cut.get(CS::CPOS);
    pos.x = Distance(x.toDouble(), currentUnit, currentResolution).in(PX);

    this->ci->cut.put(CS::CPOS, pos);
    this->ci->setPos(pos.x, pos.y);
  }
  else if(currentState == Engraving) {
    Point pos = this->ei->engraving.get(ES::EPOS);
    pos.x = Distance(x.toDouble(), currentUnit, currentResolution).in(PX);
    this->ei->engraving.put(ES::EPOS, pos);
    this->ei->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_posY_update(const QString& y)  {
  if(currentState == Cut) {
    Point pos = this->ci->cut.get(CS::CPOS);
    pos.y = Distance(y.toDouble(), currentUnit, currentResolution).in(PX);
    this->ci->cut.put(CS::CPOS, pos);
    this->ci->setPos(pos.x, pos.y);
  }
  else if(currentState == Engraving) {
    Point pos = this->ei->engraving.get(ES::EPOS);
    pos.y = Distance(y.toDouble(), currentUnit, currentResolution).in(PX);
    this->ei->engraving.put(ES::EPOS, pos);
    this->ei->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_sort_update(int s) {
  if(currentState == Cut) {
    switch(s) {
    case 0:
      this->ci->cut.put(CS::SORT, CS::SIMPLE);
        break;
    case 1:
      this->ci->cut.put(CS::SORT, CS::INNER_OUTER);
        break;
    case 2:
      this->ci->cut.put(CS::SORT, CS::SHORTEST_PATH);
        break;
    }
  }
}

void ObjectPropertyWidget::on_direction_update(int d) {
  if(currentState == Engraving) {
      if(d == 0)
        this->ei->engraving.put(ES::DIRECTION, ES::TOPDOWN);
      else
        this->ei->engraving.put(ES::DIRECTION, ES::BOTTOMUP);
  }
}

void ObjectPropertyWidget::on_dithering_update(int d) {
  if(currentState == Engraving) {
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

void ObjectPropertyWidget::on_unit_update(int d) {
	if(d == 0)
		currentUnit = MM;
	else if(d == 1)
		currentUnit = IN;
	else if(d == 2)
		currentUnit = PX;
	this->update();
}
