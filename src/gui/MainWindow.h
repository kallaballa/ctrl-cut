#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_


#include <QMainWindow>
#include "ui_CtrlCut.h"
#include <QAbstractSocket>
#include <QtGui>
#include <assert.h>
#include "LpdClient.h"
#include "StreamUtils.h"
#include "GroupItem.h"
#include "CtrlCutScene.h"
#include "LaserDialog.h"
#include "helpers/Qt.h"
#include "helpers/DocumentItem.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
public:
  ~MainWindow();

  static MainWindow *instance() { 
    if (!MainWindow::inst) MainWindow::inst = new MainWindow(); return MainWindow::inst; 
  }
public slots:
  void on_helpAboutAction_triggered();
  void on_fileOpenAction_triggered();
  void on_fileImportAction_triggered();
  void on_filePrintAction_triggered();
  void on_toolsMoveToOriginAction_triggered();
  void on_lpdclient_done(bool error);
  void on_lpdclient_progress(int done, int total);

  void sceneSelectionChanged();

  void openFile(const QString &filename);
  void importFile(const QString &filename);

private:
  static MainWindow *inst;
  MainWindow();

  LpdClient *lpdclient;
  CtrlCutScene *scene;
  DocumentItem *documentitem;
  LaserDialog *laserdialog;
};

#endif
