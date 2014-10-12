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
