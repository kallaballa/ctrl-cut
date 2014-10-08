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
