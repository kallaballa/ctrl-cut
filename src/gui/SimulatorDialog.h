#ifndef SIMULATOR_DIALOG_H_
#define SIMULATOR_DIALOG_H_


#include <qdialog.h>
#include "ui_SimulatorDialog.h"
#include <QtGui>
#include <assert.h>
#include "SimulatorScene.h"
#include "helpers/Qt.h"
#include "helpers/DocumentItem.h"
#include "Canvas.h"

class SimulatorDialog : public QDialog, public Canvas, private Ui::SimulatorDialog
{
  Q_OBJECT
public:
  SimulatorDialog(DocumentItem& documentItem, QWidget *parent = NULL);
  ~SimulatorDialog();
public slots:
  void sceneSelectionChanged();
  void simulate();
  void drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b){};
  void drawMove(coord x0, coord y0, coord x1, coord y1);
  void drawCut(coord x0, coord y0, coord x1, coord y1);
  void update(){};
  void dumpVectorImage(const string& filename, BoundingBox* clip = NULL){};
  void dumpRasterImage(const string& filename, BoundingBox* clip = NULL){};
private:
  SimulatorScene *scene;
  DocumentItem* documentItem;
  QPen movePen;
  QPen cutPen;
};

#endif
