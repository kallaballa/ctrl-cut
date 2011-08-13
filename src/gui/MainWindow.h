#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "ui_CtrlCut.h"
#include <QAbstractSocket>

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
  void on_laosclient_done(bool error);
  void on_laosclient_progress(int done, int total);

  void sceneSelectionChanged();

  void openFile(const QString &filename);

private:
  static MainWindow *inst;
  MainWindow();

  class LpdClient *lpdclient;
  class LaosClient *laosclient;
  class QGraphicsItem *firstitem;
  class CtrlCutScene *scene;

  class PostscriptParser *psparser; // Only to retain data between loads, needed by Raster
  class CutModel *cutmodel;
  class Raster *raster;
  class QGraphicsItemGroup *documentitem;
  class QGraphicsPixmapItem *rasteritem;
  class QGraphicsItemGroup *vectoritem;
  QPixmap rasterpixmap;
  QPointF rasterpos;
  class LaserDialog *laserdialog;
};

#endif
