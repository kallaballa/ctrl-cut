#include "SimulatorDialog.h"
#include "helpers/EngraveCanvas.h"
#include <Document.hpp>

typedef DocumentSettings DS;
SimulatorDialog::SimulatorDialog(Document& doc, QWidget *parent) :
QDialog(parent),
Canvas(doc.get(DS::WIDTH).in(PX),doc.get(DocumentSettings::HEIGHT).in(PX)),
doc(&doc),
movePen(Qt::white),
cutPen(Qt::red)
{
  setupUi(this);
 // this->engraveCanvas = new EngraveCanvas(21600);
  this->scene = new SimulatorScene(this);
  this->graphicsView->setScene(this->scene);

  connect(this->scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
  connect(this->scene, SIGNAL(sceneRectChanged(const QRectF&)),
          this->graphicsView, SLOT(updateSceneRect(const QRectF&)));
//  this->scene->addItem(this->engraveCanvas);
}

SimulatorDialog::~SimulatorDialog(){}
/*
void SimulatorDialog::createPixmapItem(QImage& img, Coord_t x, Coord_t y) {
  QPixmap pixmap = QPixmap::fromImage(img);
  if (!pixmap.isNull()) {
    QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(pixmap);
    pixmapItem->setPos(x,y);
    this->scene->addItem(pixmapItem);
  }
}
*/
/*
void SimulatorDialog::addImage(QImage& img, Coord_t x, Coord_t y) {
  emit createPixmapItem(img,x,y);
}

*/

void SimulatorDialog::sceneSelectionChanged()
{
  printf("selectionChanged\n");
  foreach (QGraphicsItem *item, this->scene->selectedItems()) {
    printf("item: %p\n", item);
  }
}

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "pclint/Interpreter.h"
#include "pclint/PclIntConfig.h"
#include "pclint/Plotter.h"
#include <stdlib.h>

using std::ofstream;
using std::ifstream;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

void SimulatorDialog::drawCut(coord x0, coord y0, coord x1, coord y1) {
  QGraphicsLineItem *line = new QGraphicsLineItem(x0, y0, x1, y1);
  line->setPen(this->cutPen);
  this->scene->addItem(line);
}

void SimulatorDialog::drawMove(coord x0, coord y0, coord x1, coord y1) {
  QGraphicsLineItem *line = new QGraphicsLineItem(x0, y0, x1, y1);
  line->setPen(this->movePen);
  this->scene->addItem(line);
}

void SimulatorDialog::engravePixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b) {
 // engraveCanvas->drawPixel(x0,y0, r,g,b);
}
void SimulatorDialog::drawPixel(coord x0, coord y0, uint8_t r,uint8_t g,uint8_t b) {
  //emit engravePixel(x0,y0, r,g,b);
}

void SimulatorDialog::simulate() {
  this->scene->reset();
  // REFACTOR
  //this->doc->commit();
  Document& doc = *this->doc;
  PclIntConfig* config = PclIntConfig::singleton();
  config->autocrop = true;
  config->clip = NULL;
  config->debugLevel = LVL_INFO;
  config->interactive = false;
  config->screenSize =BoundingBox::createFromGeometryString("1024x768");

  ofstream *tmpfile = new ofstream("pclint.tmp", ios::out | ios::binary);
  doc.put(DocumentSettings::ENABLE_RASTER, true);

  doc.write(*tmpfile);
  tmpfile->close();

  ifstream *infile = new ifstream("pclint.tmp", ios::in | ios::binary);
  PclIntConfig::singleton()->debugLevel = LVL_DEBUG;
  RtlPlot* plot = new RtlPlot(infile);
  Statistic::init(plot->getWidth(), plot->getHeight(), plot->getResolution());
  Interpreter& intr = * new Interpreter(plot, this);

  Debugger::create(this);
  Debugger::getInstance()->autoupdate = true;

  intr.render();


  if (config->debugLevel >= LVL_INFO) {
    Statistic::singleton()->printSlot(cout, SLOT_VECTOR);
    Statistic::singleton()->printSlot(cout, SLOT_RASTER);
  }
}
