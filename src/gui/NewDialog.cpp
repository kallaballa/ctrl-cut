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
}

NewDialog::~NewDialog() {
}

Unit NewDialog::getUnit() {
  int index = this->unit->currentIndex();
  switch (index) {
  case 0:
    return IN;
  case 1:
    return MM;
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

int NewDialog::getResolution() {
  int index = this->resolutionCombo->currentIndex();
  switch (index) {
  case 0:
    return 75;
  case 1:
    return 150;
  case 2:
    return 300;
  case 3:
    return 600;
  case 4:
    return 1200;
  }
  assert(false);
  return -1;
}

void NewDialog::loadFrom(GuiConfig& config) {
  Unit unit = config.unit;
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

  size_t res = config.resolution;
  for(size_t i = 0; i < resolutionCombo->count(); ++i) {
    if(resolutionCombo->itemText(i).toStdString() == (std::to_string(res) + " DPI")) {
      resolutionCombo->setCurrentIndex(i);
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

