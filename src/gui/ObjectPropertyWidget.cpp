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

ObjectPropertyWidget::ObjectPropertyWidget(QWidget *parent) : QWidget(parent), currentState(NONE), currentUnit(MM),currentResolution(600), doc(NULL) {
}

ObjectPropertyWidget::~ObjectPropertyWidget() {
}

void ObjectPropertyWidget::updateEngraveProperties(const EngraveSettings::KeyBase&  key) {
  //  this->enable(this->engravings[0]);
}

void ObjectPropertyWidget::updateCutProperties(const CutSettings::KeyBase&  key) {
  //  this->enable(this->cuts[0]);
}

void ObjectPropertyWidget::enable(const std::vector<AbstractCtrlCutItem*> &items) {

  std::vector<CutItem *> cutitems;
  std::vector<EngraveItem *> engraveitems;
  foreach(AbstractCtrlCutItem *item, items) {
    if (CutItem *ci = dynamic_cast<CutItem*>(item)) cutitems.push_back(ci);
    else if (EngraveItem *ei = dynamic_cast<EngraveItem*>(item)) engraveitems.push_back(ei);
  }

  if (!cutitems.empty() && engraveitems.empty()) {
    enableCutItems(cutitems);
  }
  else if (cutitems.empty() && !engraveitems.empty()) {
    enableEngraveItems(engraveitems);
  }
  else { // mixed
    disable();
  }

  if(doc) {
    setDocument(doc);
  }
}

void ObjectPropertyWidget::setDocument(Document* doc) {
  typedef DocumentSettings DS;
  this->doc = doc;
  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());
  mainw->titleEdit->setText(QString::fromStdString(doc->get(DS::TITLE)));

  if(doc->get(DS::AUTO_FOCUS))
    mainw->autofocusBox->setChecked(true);
  else
    mainw->autofocusBox->setChecked(false);
}

template<typename T>
static T getSetting(const std::vector<CutItem *> &items, const Settings::Key<T> &key, bool &single) {
  T val;
  bool first = true;
  single = true;
  foreach(CutItem *ci, items) {
    T tmp = ci->cut->get(key);
    if (first) {
      val = tmp;
      first = false;
    }
    else {
      if (val != tmp) {
        single = false;
        break;
      }
    }
  }
  return val;
}

template<typename T>
static void setSpinBoxValue(const std::vector<CutItem *> &items, const Settings::Key<T> &key, QSpinBox *widget) {
  bool single;
  auto val = getSetting(items, key, single);
  if (single) {
    widget->setEnabled(true);
    if (!widget->hasFocus()) widget->setValue(val);
  }
  else {
    widget->setEnabled(false);
  }
}

template<typename T>
void ObjectPropertyWidget::setLineEditValue(const std::vector<CutItem *> &items, const Settings::Key<T> &key, QLineEdit *widget) {
  bool single;
  auto val = getSetting(items, key, single);
  if (single) {
    widget->setEnabled(true);
    if (!widget->hasFocus()) widget->setText(QString::number(val.in(this->currentUnit)));
  }
  else {
    widget->setEnabled(false);
  }
}

void ObjectPropertyWidget::enableCutItems(const std::vector<CutItem *> &items)
{
  if (this->currentState != Cut) this->disable();

  MainWindow* mainw = qobject_cast<MainWindow*>(this->parentWidget()->parentWidget());


  this->cuts = items;

  setLineEditValue(items, CS::REDUCE, mainw->reduceEdit);
  setSpinBoxValue(items, CS::CSPEED, mainw->speed);
  setSpinBoxValue(items, CS::CPOWER, mainw->power);
  mainw->frequency->show();
  setSpinBoxValue(items, CS::FREQUENCY, mainw->frequency);

  CutItem *ci = this->cuts[0];
  ci->cut->settings.setUpdateTrigger(std::bind(&ObjectPropertyWidget::updateCutProperties, this, std::placeholders::_1));

  Point pos = ci->cut->get(CS::CPOS);
  CS::Sort sort = ci->cut->get(CS::SORT);
  this->currentResolution = ci->cut->get(DS::RESOLUTION);

  double posX = Distance(pos.x,PX,this->currentResolution).in(this->currentUnit);
  double posY = Distance(pos.y,PX,this->currentResolution).in(this->currentUnit);
  double reduce = ci->cut->get(CS::REDUCE).in(this->currentUnit);

  mainw->posX->setEnabled(true);
  mainw->posY->setEnabled(true);

  if(!mainw->posX->hasFocus())
    mainw->posX->setText(QString::number(posX));
  if(!mainw->posY->hasFocus())
    mainw->posY->setText(QString::number(posY));
  if(!mainw->reduceEdit->hasFocus())
    mainw->reduceEdit->setText(QString::number(reduce));

  mainw->reduceEdit->show();
  mainw->reduceLabel->show();
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
  default:
    assert(false && "Unhandled cut optimize setting");
    break;
  }

  mainw->sort->setCurrentIndex(mainw->sort->findText(strSort));
  this->currentState = Cut;
}

void ObjectPropertyWidget::enableEngraveItems(const std::vector<EngraveItem *> &items)
{
  if (this->currentState != Engraving) this->disable();
  this->engravings = items;
  EngraveItem *ei = this->engravings[0];
  ei->engraving->settings.setUpdateTrigger(std::bind(&ObjectPropertyWidget::updateEngraveProperties, this, std::placeholders::_1));

  Point pos = ei->engraving->get(ES::EPOS);
  int speed = ei->engraving->get(ES::ESPEED);
  int power = ei->engraving->get(ES::EPOWER);
  this->currentResolution = ei->engraving->get(DS::RESOLUTION);
  ES::Dithering dither = ei->engraving->get(ES::DITHERING);
  ES::Direction direction = ei->engraving->get(ES::DIRECTION);

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

  double posX = Distance(pos.x,PX,this->currentResolution).in(this->currentUnit);
  double posY = Distance(pos.y,PX,this->currentResolution).in(this->currentUnit);

  if(!mainw->posX->hasFocus())
    mainw->posX->setText(QString::number(posX));
  if(!mainw->posY->hasFocus())
    mainw->posY->setText(QString::number(posY));
  if(!mainw->speed->hasFocus())
    mainw->speed->setValue(speed);
  if(!mainw->power->hasFocus())
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

  if(!mainw->dithering->hasFocus())
    mainw->dithering->setCurrentIndex(mainw->dithering->findText(ditherstr));
  if(!mainw->direction->hasFocus())
    mainw->direction->setCurrentIndex(direction);
  this->currentState = Engraving;
}

void ObjectPropertyWidget::update() {
  if (!this->cuts.empty()) {
    //    this->enable(this->cuts);
  }
  if (!this->engravings.empty()) {
    //    this->enable(this->engravings);
  }
}

void ObjectPropertyWidget::disable() {
  if(this->currentState == Cut) {
    this->cuts[0]->cut->settings.deleteUpdateTrigger();
    this->cuts.clear();
  } else if(this->currentState == Engraving) {
    this->engravings[0]->engraving->settings.deleteUpdateTrigger();
    this->engravings.clear();
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
      this->cuts[0]->cut->put(CS::CPOWER, val.toInt());
    else if(currentState == Engraving) {
      this->engravings[0]->engraving->put(ES::EPOWER, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_speed_update(const QString& s)  {
  if(s.length() > 0) {
    QString val = QString(s).remove(s.length() -1,1);
    std::cerr << val.toStdString() << std::endl;
    if(currentState == Cut)
      this->cuts[0]->cut->put(CS::CSPEED, val.toInt());
    else if(currentState == Engraving) {
      this->engravings[0]->engraving->put(ES::ESPEED, val.toInt());
    }
  }
}

void ObjectPropertyWidget::on_frequency_update(const QString& f)  {
  if(f.length() > 0 && currentState == Cut) {
    QString val = QString(f).remove(f.length() -2,2);
    std::cerr << val.toStdString() << std::endl;
    this->cuts[0]->cut->put(CS::FREQUENCY, val.toInt());
  }
}

void ObjectPropertyWidget::on_posX_update(const QString& x)  {
  if(currentState == Cut) {
    Point pos = this->cuts[0]->cut->get(CS::CPOS);
    pos.x = Distance(x.toDouble(), currentUnit, currentResolution).in(PX);

    this->cuts[0]->cut->put(CS::CPOS, pos);
    this->cuts[0]->setPos(pos.x, pos.y);
  }
  else if(currentState == Engraving) {
    Point pos = this->engravings[0]->engraving->get(ES::EPOS);
    pos.x = Distance(x.toDouble(), currentUnit, currentResolution).in(PX);
    this->engravings[0]->engraving->put(ES::EPOS, pos);
    this->engravings[0]->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_posY_update(const QString& y)  {
  if(currentState == Cut) {
    Point pos = this->cuts[0]->cut->get(CS::CPOS);
    pos.y = Distance(y.toDouble(), currentUnit, currentResolution).in(PX);
    this->cuts[0]->cut->put(CS::CPOS, pos);
    this->cuts[0]->setPos(pos.x, pos.y);
  }
  else if(currentState == Engraving) {
    Point pos = this->engravings[0]->engraving->get(ES::EPOS);
    pos.y = Distance(y.toDouble(), currentUnit, currentResolution).in(PX);
    this->engravings[0]->engraving->put(ES::EPOS, pos);
    this->engravings[0]->setPos(pos.x, pos.y);
  }
}

void ObjectPropertyWidget::on_sort_update(int s) {
  if(currentState == Cut) {
    switch(s) {
    case 0:
      this->cuts[0]->cut->put(CS::SORT, CS::SIMPLE);
        break;
    case 1:
      this->cuts[0]->cut->put(CS::SORT, CS::INNER_OUTER);
        break;
    case 2:
      this->cuts[0]->cut->put(CS::SORT, CS::SHORTEST_PATH);
        break;
    case 3:
      this->cuts[0]->cut->put(CS::SORT, CS::TSP);
        break;
    }
  }
}

void ObjectPropertyWidget::on_direction_update(int d) {
  if(currentState == Engraving) {
      if(d == 0)
        this->engravings[0]->engraving->put(ES::DIRECTION, ES::TOPDOWN);
      else
        this->engravings[0]->engraving->put(ES::DIRECTION, ES::BOTTOMUP);
  }
}

void ObjectPropertyWidget::on_dithering_update(int d) {
  if(currentState == Engraving) {
    switch(d) {
    case 0:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::DEFAULT_DITHERING);
        break;
    case 1:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::BAYER);
        break;
    case 2:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::FLOYD_STEINBERG);
        break;
    case 3:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::JARVIS);
        break;
    case 4:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::STUCKI);
        break;
    case 5:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::BURKE);
        break;
    case 6:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::SIERRA2);
        break;
    case 7:
      this->engravings[0]->engraving->put(ES::DITHERING, ES::SIERRA3);
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

void ObjectPropertyWidget::on_autofocus_update(int state) {
  this->doc->put(DocumentSettings::AUTO_FOCUS, state);
}

void ObjectPropertyWidget::on_title_update(const QString& text) {
  this->doc->put(DocumentSettings::TITLE, text.toStdString());
}

void ObjectPropertyWidget::on_reduce_update(const QString& reduce) {
  if(currentState == Cut) {
    this->cuts[0]->cut->put(CS::REDUCE, Distance(reduce.toDouble(), currentUnit, currentResolution));
  }
}
