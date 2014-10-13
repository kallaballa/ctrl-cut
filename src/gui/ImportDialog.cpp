/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "ImportDialog.hpp"
#include <QString>
#include <assert.h>

ImportDialog::ImportDialog(QWidget *parent) : QDialog(parent)
{
  setupUi(this);
}

ImportDialog::~ImportDialog() {
}

bool ImportDialog::isRasterDataEnabled() {
  return this->bothButton->isChecked() || this->rasterButton->isChecked();
}

bool ImportDialog::isVectorDataEnabled() {
  return this->bothButton->isChecked() || this->vectorButton->isChecked();
}
