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
