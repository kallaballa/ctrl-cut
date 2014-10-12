#ifndef NEwDIALOG_H_
#define NEwDIALOG_H_

#include <QDialog>
#include "ui_NewDialog.h"
#include "util/Measurement.hpp"
#include "GuiConfig.hpp"

class NewDialog : public QDialog, private Ui::NewDialog
{
  Q_OBJECT
public:
  NewDialog(QWidget *parent = NULL);
  ~NewDialog();
  Unit getUnit();
  Distance getWidth();
  Distance getHeight();
  int getResolution();
  void loadFrom(GuiConfig& config);
  void saveTo(GuiConfig& config);
private:
};

#endif
