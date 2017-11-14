/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef PREVIEWDIALOG_H_
#define PREVIEWDIALOG_H_

#include <QDialog>
#include <QProgressDialog>
#include "ui_PreviewDialog.h"
#ifndef Q_MOC_RUN
#include "Document.hpp"
#endif

class PreviewDialog : public QDialog, private Ui::Dialog
{
  Q_OBJECT
public:
  PreviewDialog(Document& doc, QWidget *parent = NULL);
  ~PreviewDialog();
public slots:
  void on_zoom_triggered(const QString& s);
  void on_cancel();
private:
  QProgressDialog progressDialog;
};

#endif
