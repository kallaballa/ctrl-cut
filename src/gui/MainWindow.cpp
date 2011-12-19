#include "MainWindow.h"

MainWindow *MainWindow::inst = NULL;

MainWindow::MainWindow() : documentitem(NULL), laserdialog(NULL)
{
  this->lpdclient = new LpdClient(this);
  this->lpdclient->setObjectName("lpdclient");

  setupUi(this);

  this->scene = new CtrlCutScene(this);
  this->graphicsView->setScene(this->scene);

  connect(this->scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
  connect(this->scene, SIGNAL(sceneRectChanged(const QRectF&)),
          this->graphicsView, SLOT(updateSceneRect(const QRectF&)));

  connect(this->graphicsView, SIGNAL(fileDropped(const QString &)), 
          this, SLOT(openFile(const QString &)));
}

MainWindow::~MainWindow()
{}

void MainWindow::openFile(const QString &filename)
{
  if (!filename.isEmpty()) {
    if(this->documentitem != NULL) {
      delete this->documentitem;
    }
    Document& loading = * new Document();
    loading.settings.put(EngraveSettings::DITHERING, EngraveSettings::BAYER);
    loading.settings.put(DocumentSettings::LOAD_ENGRAVING, true);
    loading.load(filename.toStdString());
    this->documentitem = new DocumentItem(*this->scene,loading);
  }
}

void MainWindow::importFile(const QString &filename)
{
  if (!filename.isEmpty()) {
    Document& loading = * new Document();
    loading.settings.put(EngraveSettings::DITHERING, EngraveSettings::BAYER);
    loading.settings.put(DocumentSettings::LOAD_ENGRAVING, true);

    loading.load(filename.toStdString());
    if(this->documentitem == NULL) {
      this->documentitem = new DocumentItem(*this->scene,loading);
    } else {
      this->documentitem->load(loading);
    }
  }
}

void MainWindow::on_fileOpenAction_triggered()
{
  openFile(QFileDialog::getOpenFileName(this, "Open File", "", "Ctrl-Cut SVG (*.svg)"));
}

void MainWindow::on_fileImportAction_triggered()
{
  importFile(QFileDialog::getOpenFileName(this, "Import File", "", "Supported files (*.ps *.vector *.svg)"));
}

void MainWindow::on_filePrintAction_triggered()
{
  if (!this->documentitem) {
    fprintf(stderr, "No document loaded\n");
    return;
  }

  if (!this->laserdialog) this->laserdialog = new LaserDialog(this);
  if (this->laserdialog->exec() != QDialog::Accepted) return;

  this->laserdialog->updateLaserConfig(this->documentitem->doc);
  this->documentitem->doc.settings.put(DocumentSettings::TITLE, "Default Title");
  this->documentitem->doc.settings.put(DocumentSettings::USER, "Default User");
  QStringList items;
  items << "Lazzzor" << "localhost";
  bool ok;
  QString item = QInputDialog::getItem(this, "Send to where?", "Send to where?", items, 0, false, &ok);
  if (ok && !item.isEmpty()) {
    QString host = (item == "Lazzzor")?"10.20.30.27":"localhost";

    QByteArray rtlbuffer;
    ByteArrayOStreambuf streambuf(rtlbuffer);
    std::ostream ostream(&streambuf);
    this->documentitem->commit();
    this->documentitem->doc.preprocess();
    this->documentitem->doc.write(ostream);
    
    this->lpdclient->print(host, "MyDocument", rtlbuffer);
  }
}

void MainWindow::on_lpdclient_done(bool error)
{
  if (error) fprintf(stderr, "LPD error\n");
  else printf("LPD done\n");
}

void MainWindow::on_lpdclient_progress(int done, int total)
{
  printf("Progress: %.0f%%\n", 100.0f*done/total);
}

void MainWindow::sceneSelectionChanged()
{
  printf("selectionChanged\n");
  foreach (QGraphicsItem *item, this->scene->selectedItems()) {
    printf("item: %p\n", item);
  }
}

void MainWindow::on_toolsMoveToOriginAction_triggered()
{
  /* REFACTOR
  QRectF brect = this->documentitem->boundingRect();
  qDebug() << "brect: " << brect.topLeft().x() << "," << brect.topLeft().y();
  QPointF topleft = brect.topLeft();
  this->documentitem->setPos(-topleft);
  */
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

void
MainWindow::on_helpAboutAction_triggered()
{
  qApp->setWindowIcon(QApplication::windowIcon());
  QMessageBox::information(this, "About Ctrl-Cut", 
                           QString("Ctrl-Cut " TOSTRING(CTRLCUT_VERSION) " (http://github.com/metalab/ctrl-cut)\n") +
                           QString("Copyright (C) 2009-2011 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>\n"
                                   "\n"
                                   "This program is free software; you can redistribute it and/or modify"
                                   "it under the terms of the GNU General Public License as published by"
                                   "the Free Software Foundation; either version 2 of the License, or"
                                   "(at your option) any later version."));
}

void
MainWindow::on_simulateAction_triggered()
{
  simulate();
}

#undef Point
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Interpreter.h"
#include "PclIntConfig.h"
#include "Plotter.h"
#include "SDLCanvas.h"
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

void MainWindow::simulate() {
  PclIntConfig* config = PclIntConfig::singleton();
  config->autocrop = true;
  config->clip = NULL;
  config->debugLevel = LVL_INFO;
  config->interactive = false;
  config->screenSize =BoundingBox::createFromGeometryString("1024x768");

  ofstream *tmpfile = new ofstream("pclint.tmp", ios::out | ios::binary);
  bool oldWriteEngraving = this->documentitem->doc.settings.get(DocumentSettings::ENABLE_RASTER);
  this->documentitem->doc.settings.put(DocumentSettings::ENABLE_RASTER, true);
  this->documentitem->doc.preprocess();
  this->documentitem->doc.write(*tmpfile);
  this->documentitem->doc.settings.put(DocumentSettings::ENABLE_RASTER, oldWriteEngraving);

  ifstream *infile = new ifstream("pclint.tmp", ios::in | ios::binary);
  RtlPlot* plot = new RtlPlot(infile);
  Statistic::init(plot->getWidth(), plot->getHeight(), plot->getResolution());
  SDLCanvas* canvas = NULL;
  if(PclIntConfig::singleton()->screenSize != NULL)
    canvas = new SDLCanvas(plot->getWidth(), plot->getHeight(), PclIntConfig::singleton()->screenSize->ul.x, PclIntConfig::singleton()->screenSize->ul.y);
  else
    canvas = new SDLCanvas(plot->getWidth(), plot->getHeight());

  Interpreter& intr = * new Interpreter(plot, canvas);
  boost::thread aborted_thread(boost::bind(sdl_wait, &intr));

  Debugger::create(canvas);
  Debugger::getInstance()->autoupdate = true;

  boost::thread render_thread(boost::bind(intr_render_sqlquit, &intr));
  render_thread.join();

  if (config->debugLevel >= LVL_INFO) {
    Statistic::singleton()->printSlot(cout, SLOT_VECTOR);
    Statistic::singleton()->printSlot(cout, SLOT_RASTER);
  }
}
