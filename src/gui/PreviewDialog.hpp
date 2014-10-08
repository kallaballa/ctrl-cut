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
private:
};

#endif
