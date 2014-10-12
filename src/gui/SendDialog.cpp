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

void SendDialog::loadFrom(GuiConfig& config) {
  netAddressEdit->setText(QString::fromStdString(config.networkAddress));
}

void SendDialog::saveTo(GuiConfig& config) {
  config.networkAddress = getNetworkAddress().toStdString();
}
