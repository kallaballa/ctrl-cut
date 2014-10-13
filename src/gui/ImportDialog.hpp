/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef IMPORTDIALOG_H_
#define IMPORTDIALOG_H_

#include <QDialog>
#include "ui_ImportDialog.h"

class ImportDialog : public QDialog, private Ui::ImportDialog
{
  Q_OBJECT
public:
  ImportDialog(QWidget *parent = NULL);
  ~ImportDialog();
  bool isVectorDataEnabled();
  bool isRasterDataEnabled();
private:
};

#endif
