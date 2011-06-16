#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "ui_MainWindow.h"
#include <QAbstractSocket>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

public slots:
  void on_fileOpenAction_triggered();
  void on_filePrintAction_triggered();
  void on_lpdclient_done(bool error);
  void on_lpdclient_progress(int done, int total);

private:
  class LpdClient *lpdclient;
};

#endif
