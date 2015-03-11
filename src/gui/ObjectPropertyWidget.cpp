/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "ObjectPropertyWidget.hpp"
#include "MainWindow.hpp"
#include "config/EngraveSettings.hpp"
#include "config/CutSettings.hpp"
#include "config/DocumentSettings.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include "Document.hpp"
#include <QUndoView>
#include <assert.h>

typedef DocumentSettings DS;
typedef EngraveSettings ES;
typedef CutSettings CS;

ObjectPropertyWidget::ObjectPropertyWidget(QWidget *parent) : QWidget(parent), currentState(NONE), currentUnit(MM),currentResolution(600), doc(NULL), ci(NULL), ei(NULL) {
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
  EngraveItem *ei;
  CutItem *ci;

  if ((ei = dynamic_cast<EngraveItem*>(item))) {
    enableEngraveItem(ei);
  }
  else if ((ci = dynamic_cast<CutItem*>(item))) {
    enableCutItem(ci);
  }

  if(doc) {
    setDocument(doc);
  }
}

void ObjectPropertyWidget::setDocument(DocumentPtr doc) {
  typedef DocumentSettings DS;
  this->doc = doc;
  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());
  mainw->titleEdit->setText(QString::fromStdString(doc->get(DS::TITLE)));

  if(doc->get(DS::AUTO_FOCUS))
    mainw->autofocusBox->setChecked(true);
  else
    mainw->autofocusBox->setChecked(false);

  mainw->center->setCurrentIndex(mainw->center->findText(QString::fromStdString(DS::getCenterName(doc->get(DS::CENTER)))));
  mainw->airAssistBox->setCurrentIndex(mainw->airAssistBox->findText(QString::fromStdString(DS::getAirAssistName(doc->get(DS::AIR_ASSIST)))));

}

void ObjectPropertyWidget::enableCutItem(CutItem* ci) {
  if(this->currentState != Cut)
    this->disable();
  this->ci = ci;
  this->ci->cut->settings.setUpdateTrigger(std::bind(&ObjectPropertyWidget::updateCutProperties, this, std::placeholders::_1));

  Point pos = this->ci->cut->get(CS::CPOS);
  int speed = this->ci->cut->get(CS::CSPEED);
  int power = this->ci->cut->get(CS::CPOWER);
  int freq = this->ci->cut->get(CS::FREQUENCY);
  CS::Sort sort = this->ci->cut->get(CS::SORT);
  this->currentResolution = this->ci->cut->get(DS::RESOLUTION);

  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());
  Box bbox = find_bounding_box(*ci->cut.get());
  double width = Distance(bbox.width(),PX,this->currentResolution).in(this->currentUnit);
  double height = Distance(bbox.height(),PX,this->currentResolution).in(this->currentUnit);
  double posX = Distance(pos.x,PX,this->currentResolution).in(this->currentUnit);
  double posY = Distance(pos.y,PX,this->currentResolution).in(this->currentUnit);
  double reduce = this->ci->cut->get(CS::REDUCE).in(this->currentUnit);

  mainw->posX->setEnabled(true);
  mainw->posY->setEnabled(true);
  mainw->speed->setEnabled(true);
  mainw->power->setEnabled(true);

  if(!mainw->posX->hasFocus())
    mainw->posX->setText(QString::number(posX));
  if(!mainw->posY->hasFocus())
    mainw->posY->setText(QString::number(posY));
  if(!mainw->speed->hasFocus())
    mainw->speed->setValue(speed);
  if(!mainw->power->hasFocus())
    mainw->power->setValue(power);
  if(!mainw->frequency->hasFocus())
    mainw->frequency->setValue(freq);
  if(!mainw->reduceEdit->hasFocus())
    mainw->reduceEdit->setText(QString::number(reduce));

  mainw->widthEdit->setText(QString::number(width));
  mainw->heightEdit->setText(QString::number(height));

  mainw->reduceEdit->show();
  mainw->reduceLabel->show();
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
  case CS::TSP:
    strSort = "TSP";
    break;
  case CS::PASS_THROUGH:
    strSort = "Pass-through";
    break;
  default:
    assert(false && "Unhandled cut optimize setting");
    break;
  }

  mainw->sort->setCurrentIndex(mainw->sort->findText(strSort));
  this->currentState = Cut;
}

void ObjectPropertyWidget::enableEngraveItem(EngraveItem* ei) {
  if(this->currentState != Engraving)
    this->disable();
  this->ei = ei;
  this->ei->engraving->settings.setUpdateTrigger(std::bind(&ObjectPropertyWidget::updateEngraveProperties, this, std::placeholders::_1));

  Point pos = this->ei->engraving->get(ES::EPOS);
  int speed = this->ei->engraving->get(ES::ESPEED);
  int power = this->ei->engraving->get(ES::EPOWER);
  this->currentResolution = this->ei->engraving->get(DS::RESOLUTION);
  ES::Dithering dither = this->ei->engraving->get(ES::DITHERING);
  ES::Direction direction = this->ei->engraving->get(ES::DIRECTION);

  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());

  mainw->posX->setEnabled(true);
  mainw->posY->setEnabled(true);
  mainw->speed->setEnabled(true);
  mainw->power->setEnabled(true);

  mainw->reduceEdit->hide();
  mainw->reduceLabel->hide();
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

  Box bbox = find_bounding_box(*ei->engraving.get());
  double posX = Distance(pos.x,PX,this->currentResolution).in(this->currentUnit);
  double posY = Distance(pos.y,PX,this->currentResolution).in(this->currentUnit);
  double width = Distance(bbox.width(),PX,this->currentResolution).in(this->currentUnit);
  double height = Distance(bbox.height(),PX,this->currentResolution).in(this->currentUnit);

  if(!mainw->posX->hasFocus())
    mainw->posX->setText(QString::number(posX));
  if(!mainw->posY->hasFocus())
    mainw->posY->setText(QString::number(posY));
  if(!mainw->speed->hasFocus())
    mainw->speed->setValue(speed);
  if(!mainw->power->hasFocus())
    mainw->power->setValue(power);

  mainw->widthEdit->setText(QString::number(width));
  mainw->heightEdit->setText(QString::number(height));

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
  default:
    assert(false && "Unhandled raster dithering setting");
    break;
  }

  if(!mainw->dithering->hasFocus())
    mainw->dithering->setCurrentIndex(mainw->dithering->findText(ditherstr));
  if(!mainw->direction->hasFocus())
    mainw->direction->setCurrentIndex(direction);
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
    this->ci->cut->settings.deleteUpdateTrigger();
    this->ci = NULL;
  } else if(this->currentState == Engraving) {
    this->ei->engraving->settings.deleteUpdateTrigger();
    this->ei = NULL;
  }
  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());
  mainw->posX->setEnabled(false);
  mainw->posY->setEnabled(false);
  mainw->speed->setEnabled(false);
  mainw->power->setEnabled(false);
  mainw->reduceEdit->hide();
  mainw->reduceLabel->hide();
  mainw->frequency->hide();
  mainw->freqLabel->hide();
  mainw->sort->hide();
  mainw->sortLabel->hide();
  mainw->ditherLabel->hide();
  mainw->direction->hide();
  mainw->directionLabel->hide();
  mainw->dithering->hide();
  mainw->ditherLabel->hide();
  mainw->direction->hide();
  mainw->directionLabel->hide();
  this->currentState = NONE;
}

void ObjectPropertyWidget::on_power_update(const QString& p) {
  if(p.length() > 0) {
    QString val = QString(p).remove(p.length() -1,1);
    std::cerr << val.toStdString() << std::endl;
    if(currentState == Cut)
      this->ci->cut->put(CS::CPOWER, val.toInt());
    else if(currentState == Engraving) {
      this->ei->engraving->put(ES::EPOWER, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_speed_update(const QString& s)  {
  if(s.length() > 0) {
    QString val = QString(s).remove(s.length() -1,1);
    std::cerr << val.toStdString() << std::endl;
    if(currentState == Cut)
      this->ci->cut->put(CS::CSPEED, val.toInt());
    else if(currentState == Engraving) {
      this->ei->engraving->put(ES::ESPEED, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_frequency_update(const QString& f)  {
  if(f.length() > 0 && currentState == Cut) {
    QString val = QString(f).remove(f.length() -2,2);
    std::cerr << val.toStdString() << std::endl;
    this->ci->cut->put(CS::FREQUENCY, val.toInt());
  }
}

void ObjectPropertyWidget::on_posX_update(const QString& x)  {
  if(currentState == Cut) {
    Point pos = this->ci->cut->get(CS::CPOS);
    pos.x = Distance(x.toDouble(), currentUnit, currentResolution).in(PX);

    this->ci->cut->put(CS::CPOS, pos);
    this->ci->setPos(pos.x, pos.y);
  }
  else if(currentState == Engraving) {
    Point pos = this->ei->engraving->get(ES::EPOS);
    pos.x = Distance(x.toDouble(), currentUnit, currentResolution).in(PX);
    this->ei->engraving->put(ES::EPOS, pos);
    this->ei->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_posY_update(const QString& y)  {
  if(currentState == Cut) {
    Point pos = this->ci->cut->get(CS::CPOS);
    pos.y = Distance(y.toDouble(), currentUnit, currentResolution).in(PX);
    this->ci->cut->put(CS::CPOS, pos);
    this->ci->setPos(pos.x, pos.y);
  }
  else if(currentState == Engraving) {
    Point pos = this->ei->engraving->get(ES::EPOS);
    pos.y = Distance(y.toDouble(), currentUnit, currentResolution).in(PX);
    this->ei->engraving->put(ES::EPOS, pos);
    this->ei->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_sort_update(int s) {
  if(currentState == Cut) {
    switch(s) {
    case 0:
      this->ci->cut->put(CS::SORT, CS::INNER_OUTER);
        break;
    case 1:
      this->ci->cut->put(CS::SORT, CS::TSP);
        break;
    case 2:
      this->ci->cut->put(CS::SORT, CS::PASS_THROUGH);
        break;
    }
  }
}

void ObjectPropertyWidget::on_direction_update(int d) {
  if(currentState == Engraving) {
      if(d == 0)
        this->ei->engraving->put(ES::DIRECTION, ES::TOPDOWN);
      else
        this->ei->engraving->put(ES::DIRECTION, ES::BOTTOMUP);
  }
}

void ObjectPropertyWidget::on_dithering_update(int d) {
  if(currentState == Engraving) {
    switch(d) {
    case 0:
      this->ei->engraving->put(ES::DITHERING, ES::BAYER);
        break;
    case 1:
      this->ei->engraving->put(ES::DITHERING, ES::FLOYD_STEINBERG);
        break;
    case 2:
      this->ei->engraving->put(ES::DITHERING, ES::JARVIS);
        break;
    case 3:
      this->ei->engraving->put(ES::DITHERING, ES::STUCKI);
        break;
    case 4:
      this->ei->engraving->put(ES::DITHERING, ES::BURKE);
        break;
    case 5:
      this->ei->engraving->put(ES::DITHERING, ES::SIERRA2);
        break;
    case 6:
      this->ei->engraving->put(ES::DITHERING, ES::SIERRA3);
        break;
    }
  }
}

void ObjectPropertyWidget::on_airAssist_update(int d) {
  typedef DocumentSettings DS;
  DS::AirAssist aa;
  switch(d) {
    case 0:
      aa = DS::GLOBAL;
      break;
    case 1:
      aa = DS::RASTER_ONLY;
      break;
    case 2:
      aa = DS::CUT_ONLY;
      break;
    case 3:
      aa = DS::OFF;
      break;
    }

    this->doc->put(DocumentSettings::AIR_ASSIST, aa);
}
void ObjectPropertyWidget::on_center_update(int d) {
  typedef DocumentSettings DS;
  DS::Center c;
  switch(d) {
    case 0:
      c = DS::CENTER_NONE;
      break;
    case 1:
      c = DS::CENTER_CENTER;
      break;
    case 2:
      c = DS::CENTER_LEFT;
      break;
    case 3:
      c = DS::CENTER_RIGHT;
      break;
    case 4:
      c = DS::CENTER_TOP;
      break;
    case 5:
      c = DS::CENTER_BOTTOM;
      break;
    }

    this->doc->put(DocumentSettings::CENTER, c);
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

void ObjectPropertyWidget::on_autofocus_update(int state) {
  this->doc->put(DocumentSettings::AUTO_FOCUS, state);
}

void ObjectPropertyWidget::on_title_update(const QString& text) {
  this->doc->put(DocumentSettings::TITLE, text.toStdString());
}

void ObjectPropertyWidget::on_reduce_update(const QString& reduce) {
  if(currentState == Cut) {
    this->ci->cut->put(CS::REDUCE, Distance(reduce.toDouble(), currentUnit, currentResolution));
  }
}
