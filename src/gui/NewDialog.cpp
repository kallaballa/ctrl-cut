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
