#include "SendDialog.hpp"
#include <QString>
#include <assert.h>

SendDialog::SendDialog(QWidget *parent) : QDialog(parent)
{
  setupUi(this);
}

SendDialog::~SendDialog() {
}


QString SendDialog::getNetworkAddress() {
  return netAddressEdit->text();
}
