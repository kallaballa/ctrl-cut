/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "NewDialog.hpp"
#include <QString>
#include <assert.h>

NewDialog::NewDialog(QWidget *parent) : QDialog(parent)
{
  setupUi(this);
  QObject::connect(unit, SIGNAL(currentIndexChanged(int)),
      this, SLOT(on_unitChange()));
  currentUnit = getUnit();
}

NewDialog::~NewDialog() {
}

Unit NewDialog::getUnit() {
  int index = this->unit->currentIndex();
  switch (index) {
  case 0:
    return MM;
  case 1:
    return IN;
  case 2:
    return PX;
  }
  assert(false);
  return IN;
}

Distance NewDialog::getWidth() {
  double w = this->widthLine->text().toDouble();
  return Distance(w, getUnit(), getResolution());
}

Distance NewDialog::getHeight() {
  double h = this->heightLine->text().toDouble();
  return Distance(h, getUnit(), getResolution());
}

double NewDialog::getResolution() {
    return 25400;
}

void NewDialog::loadFrom(GuiConfig& config) {
  Unit unit = config.unit;
  currentUnit = unit;
  string strUnit;
  switch (unit) {
  case MM:
    strUnit = "MM";
    break;
  case PX:
    strUnit = "PX";
    break;
  case IN:
    strUnit = "IN";
    break;
  }

  for (size_t i = 0; i < this->unit->count(); ++i) {
    if (this->unit->itemText(i).toStdString() == strUnit) {
      this->unit->setCurrentIndex(i);
      break;
    }
  }

  this->widthLine->setText(QString::fromStdString(std::to_string(config.bedWidth)));
  this->heightLine->setText(QString::fromStdString(std::to_string(config.bedHeight)));
}

void NewDialog::saveTo(GuiConfig& config) {
  config.unit = getUnit();
  config.resolution = getResolution();
  config.bedWidth = getWidth().in(getUnit());
  config.bedHeight = getHeight().in(getUnit());
}

void NewDialog::on_unitChange() {
  double w = this->widthLine->text().toDouble();
  double h = this->heightLine->text().toDouble();

  this->widthLine->setText(QString::number(Distance(w, currentUnit, getResolution()).in(getUnit())));
  this->heightLine->setText(QString::number(Distance(h, currentUnit, getResolution()).in(getUnit())));
  this->currentUnit = getUnit();
}


