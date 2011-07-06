#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include "ui_CtrlCut.h"
#include <QAbstractSocket>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

public slots:
  void on_fileOpenAction_triggered();
  void on_fileImportAction_triggered();
  void on_filePrintAction_triggered();
  void on_toolsMoveToOriginAction_triggered();
  void on_lpdclient_done(bool error);
  void on_lpdclient_progress(int done, int total);

  void sceneSelectionChanged();

private:
  class LpdClient *lpdclient;
  class QGraphicsItem *firstitem;
  class CtrlCutScene *scene;

  class PostscriptParser *psparser; // Only to retain data between loads, needed by Raster
  class CutModel *cutmodel;
  class Raster *raster;
  class QGraphicsItemGroup *documentitem;
  class QGraphicsPixmapItem *rasteritem;
  QPixmap rasterpixmap;
  QPointF rasterpos;
  class LaserDialog *laserdialog;
};

#endif
