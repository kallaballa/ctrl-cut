#ifndef PREVIEWDIALOG_H_
#define PREVIEWDIALOG_H_

#include <QDialog>
#include "ui_PreviewDialog.h"
#include "Document.hpp"

class PreviewDialog : public QDialog, private Ui::Dialog
{
  Q_OBJECT
public:
  PreviewDialog(Document& doc, QWidget *parent = NULL);
  ~PreviewDialog();
public slots:
  void on_zoom_triggered(const QString& s);
private:
};

#endif
