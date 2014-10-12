#ifndef SENDDIALOG_H_
#define SENDDIALOG_H_

#include <QDialog>
#include "ui_SendDialog.h"
#include "GuiConfig.hpp"


class SendDialog : public QDialog, private Ui::SendDialog
{
  Q_OBJECT
public:
  SendDialog(QWidget *parent = NULL);
  ~SendDialog();
  QString getNetworkAddress();
  void loadFrom(GuiConfig& config);
  void saveTo(GuiConfig& config);
private:
};

#endif
