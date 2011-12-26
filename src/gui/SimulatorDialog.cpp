#include "SimulatorDialog.h"
#include "qgraphicsitem.h"

SimulatorDialog::SimulatorDialog(DocumentItem& documentItem, QWidget *parent) :
QDialog(parent), Canvas(1,1), documentItem(&documentItem), movePen(Qt::white), cutPen(Qt::red)
{
  setupUi(this);
  this->scene = new SimulatorScene(this);
  this->graphicsView->setScene(this->scene);

  connect(this->scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
  connect(this->scene, SIGNAL(sceneRectChanged(const QRectF&)),
          this->graphicsView, SLOT(updateSceneRect(const QRectF&)));
}

SimulatorDialog::~SimulatorDialog()
{}

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
#include "Interpreter.h"
#include "PclIntConfig.h"
#include "Plotter.h"
#include <stdlib.h>
#ifdef PCLINT_USE_SDL
#include <SDL.h>
#include "boost/thread.hpp"
#include "boost/bind.hpp"
#endif

using std::ofstream;
using std::ifstream;
using std::string;
using std::cerr;
using std::cout;
using std::endl;

void sdl_wait(Interpreter* intr) {
  SDL_Event event;
  do {
    SDL_PollEvent( &event );
  } while(event.type == 4 || event.type == 1);
  intr->abort();
}

void intr_render_sqlquit(Interpreter* intr) {
  intr->render();
}

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


void SimulatorDialog::simulate() {
  this->scene->reset();
  this->documentItem->commit();
  PclIntConfig* config = PclIntConfig::singleton();
  config->autocrop = true;
  config->clip = NULL;
  config->debugLevel = LVL_INFO;
  config->interactive = false;
  config->screenSize =BoundingBox::createFromGeometryString("1024x768");

  ofstream *tmpfile = new ofstream("pclint.tmp", ios::out | ios::binary);
  bool oldWriteEngraving = this->documentItem->doc.get(DocumentSettings::ENABLE_RASTER);
  this->documentItem->doc.put(DocumentSettings::ENABLE_RASTER, true);

  Document& preprocessed = this->documentItem->doc.preprocess();
  preprocessed.write(*tmpfile);
  preprocessed.put(DocumentSettings::ENABLE_RASTER, oldWriteEngraving);

  ifstream *infile = new ifstream("pclint.tmp", ios::in | ios::binary);
  RtlPlot* plot = new RtlPlot(infile);
  Statistic::init(plot->getWidth(), plot->getHeight(), plot->getResolution());
  Interpreter& intr = * new Interpreter(plot, this);

  Debugger::create(this);
  Debugger::getInstance()->autoupdate = true;

  boost::thread render_thread(&Interpreter::render, intr);
  render_thread.join();

  if (config->debugLevel >= LVL_INFO) {
    Statistic::singleton()->printSlot(cout, SLOT_VECTOR);
    Statistic::singleton()->printSlot(cout, SLOT_RASTER);
  }
}
