#ifndef SIMULATOR_DIALOG_H_
#define SIMULATOR_DIALOG_H_


#include <qdialog.h>
#include "ui_SimulatorDialog.h"
#include <QtGui>
#include "qgraphicsitem.h"
#include <assert.h>
#include "helpers/Qt.h"
#include "SimulatorScene.h"
#include "Canvas.h"

class Document;
class SimulatorDialog : public QDialog, public Canvas, private Ui::SimulatorDialog
{
  Q_OBJECT
public:
  SimulatorDialog(Document& doc, QWidget *parent = NULL);
  ~SimulatorDialog();


public slots:
  void sceneSelectionChanged();
  void simulate();
  /*
  void createPixmapItem(QImage& img, Coord_t x, Coord_t y);
  void addImage(QImage& img, Coord_t x, Coord_t y);
 */
  void engravePixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b);
  void drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b);
  void drawMove(coord x0, coord y0, coord x1, coord y1);
  void drawCut(coord x0, coord y0, coord x1, coord y1);
  void update(){};
  void dumpVectorImage(const string& filename, BoundingBox* clip = NULL){};
  void dumpRasterImage(const string& filename, BoundingBox* clip = NULL){};
private:
  Document* doc;
  SimulatorScene *scene;
  class EngraveCanvas* engraveCanvas;
  QPen movePen;
  QPen cutPen;
};

#endif
