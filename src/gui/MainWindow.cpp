#include "MainWindow.h"
#include <QtGui>

#include "FileParser.h"
#include "LaserJob.h"
#include "Driver.h"
#include "vector/model/CutModel.h"
#include "vector/geom/Geometry.h"

#include "LpdClient.h"
#include "LaosClient.h"
#include "vector/LaosEncoder.h"
#include "StreamUtils.h"
#include "GroupItem.h"
#include "CtrlCutScene.h"

#include "LaserDialog.h"

#include <assert.h>
#include <boost/format.hpp>

using boost::format;

MainWindow *MainWindow::inst = NULL;

MainWindow::MainWindow() : psparser(NULL), cutmodel(NULL), raster(NULL), documentitem(NULL), rasteritem(NULL), vectoritem(NULL), laserdialog(NULL)
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
{
}

void MainWindow::openFile(const QString &filename)
{
  if (!filename.isEmpty()) {
    if (this->cutmodel) {
      delete this->cutmodel;
      this->cutmodel = NULL;
    }
    if (this->raster) {
      delete this->raster;
      this->raster = NULL;
    }
    if (this->psparser) {
      delete this->psparser;
      this->psparser = NULL;
    }

    QFileInfo finfo(filename);
    QString suffix = finfo.suffix();
    if (suffix == "ps") {
      this->psparser = new PostscriptParser(LaserConfig::inst());
      switch (LaserConfig::inst().raster_dithering) {
      case LaserConfig::DITHER_DEFAULT:
        psparser->setRasterFormat(PostscriptParser::BITMAP);
        break;
      case LaserConfig::DITHER_BAYER:
      case LaserConfig::DITHER_FLOYD_STEINBERG:
      case LaserConfig::DITHER_JARVIS:
      case LaserConfig::DITHER_BURKE:
      case LaserConfig::DITHER_STUCKI:
      case LaserConfig::DITHER_SIERRA2:
      case LaserConfig::DITHER_SIERRA3:
        psparser->setRasterFormat(PostscriptParser::GRAYSCALE);
        break;
        
      default:
        assert(false);
      }
      cups_file_t *input_file = cupsFileOpen(filename.toLocal8Bit(), "r");
      if (!input_file) {
        LOG_ERR("Error opening postscript file");
        return;
      }
      LaserConfig::inst().basename = finfo.baseName().toStdString();
      if (!psparser->parse(input_file)) {
        LOG_FATAL("Error processing postscript");
        return;
      }

      this->cutmodel = CutModel::load(psparser->getVectorData());

      if (!this->cutmodel && !psparser->hasBitmapData()) {
        fprintf(stderr, "Error: Unable to open postscript file\n");
        return;
      }

      this->rasterpixmap = QPixmap();
      this->rasterpos = QPointF();
      if (psparser->hasBitmapData()) {
        AbstractImage *image = psparser->getImage();
        this->raster = new Raster(image);
        QImage *img;
        BitmapImage *bitmap = dynamic_cast<BitmapImage*>(image);
        if (bitmap) {
          img = new QImage((const uchar *)image->data(), image->width(), image->height(), 
                           image->rowstride(), QImage::Format_Mono);
          QVector<QRgb> colortable;
          colortable.append(qRgba(0,0,0,255));
          colortable.append(qRgba(0,0,0,0));
          img->setColorTable(colortable);
        }
        else {
          GrayscaleImage *gsimage =  dynamic_cast<GrayscaleImage*>(image);
          if (gsimage) {
            img = new QImage((const uchar *)image->data(), image->width(), image->height(), 
                             image->rowstride(), QImage::Format_Indexed8);
            QVector<QRgb> colortable;
            for(int i=0;i<255;i++) colortable.append(qRgba(i,i,i,255));
            colortable.append(qRgba(255,255,255,0));
            img->setColorTable(colortable);
          }
        }
        // Makes a deep copy of the image so it's safe for PostscriptParser to dispose
        // of its buffer
        img->bits();
        this->rasterpixmap = QPixmap::fromImage(*img);
        this->rasterpos = QPointF(image->xPos(), image->yPos());
      }
    }
    else if (suffix == "vector") {
      this->cutmodel = CutModel::load(filename.toStdString());
      if (!this->cutmodel) {
        fprintf(stderr, "Error: Unable to open vector file\n");
        return;
      }
    }

    this->documentitem = new QGraphicsItemGroup();
    this->documentitem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    this->scene->addItem(this->documentitem);

    if (this->cutmodel) {
      this->vectoritem = new QGraphicsItemGroup(this->documentitem);
      this->firstitem = NULL;
      for (CutModel::iterator iter = this->cutmodel->begin(); iter != this->cutmodel->end(); iter++) {
        const Segment &segment = **iter;
        QGraphicsLineItem *line = 
          new QGraphicsLineItem(segment[0][0], segment[0][1], segment[1][0], segment[1][1],
                                this->vectoritem);
        this->documentitem->addToGroup(line);
        if (!this->firstitem) this->firstitem = line;
      }
      this->documentitem->addToGroup(this->vectoritem);
    }

    if (!this->rasterpixmap.isNull()) {
      this->rasteritem = new QGraphicsPixmapItem(this->rasterpixmap, this->documentitem);
      this->rasteritem->setPos(this->rasterpos);
      this->documentitem->addToGroup(this->rasteritem);
    }
  }
}

void MainWindow::on_fileOpenAction_triggered()
{
  openFile(QFileDialog::getOpenFileName(this, "Open File", "", "Supported files (*.ps *.vector)"));
}

void MainWindow::on_fileImportAction_triggered()
{
  QMessageBox::information(this, "Not Implemented", "Not Implemented");
}

void MainWindow::on_filePrintAction_triggered()
{
  if (!this->cutmodel && !this->raster) {
    fprintf(stderr, "No model loaded\n");
    return;
  }

  if (!this->laserdialog) this->laserdialog = new LaserDialog(this);
  if (this->laserdialog->exec() != QDialog::Accepted) return;

  this->laserdialog->updateLaserConfig(LaserConfig::inst());
  LaserConfig::inst().dumpDebug();

  QStringList items;
  items << "Lazzzor" << "LAOS" << "localhost" << "file";
  bool ok;
  QString item = QInputDialog::getItem(this, "Send to where?", "Send to where?", items, 0, false, &ok);
  if (ok && !item.isEmpty()) {
    LaserJob job(&LaserConfig::inst(), "kintel", "jobname", "jobtitle");
    LaosEncoder laosencoder(&LaserConfig::inst());
    job.setVectorEncoder(&laosencoder);

    // Apply transformations and add to job
    QPointF pos = this->documentitem->pos();
    if (this->cutmodel) {
      QPointF vectorpos = this->vectoritem->pos() + pos;
      this->cutmodel->setTranslation(Point(vectorpos.x(), vectorpos.y()));
      LOG_DEBUG(str(format("vector translation: (%d, %d)") % vectorpos.x() % vectorpos.y()));
      job.addCut(this->cutmodel);
    }
    QPointF rasterpos = this->rasteritem->pos() + pos;
    if (this->raster) {
      this->raster->sourceImage()->setTranslation(rasterpos.x(), rasterpos.y());
      LOG_DEBUG(str(format("raster translation: (%d, %d)") % rasterpos.x() % rasterpos.y()));
      job.addRaster(this->raster);
    }

    Driver drv;
    QByteArray rtlbuffer;
    ByteArrayOStreambuf streambuf(rtlbuffer);
    std::ostream ostream(&streambuf);
    drv.process(&job, ostream);
    
    if (item == "file") {
      QString new_filename = QFileDialog::getSaveFileName(this, "Save RTL File",
                                                          "ctrl-cut.rtl",
                                                          "RTL Files (*.rtl)");
      if (!new_filename.isEmpty()) {
        QFile rtlfile(new_filename);
        if (!rtlfile.open(QIODevice::WriteOnly)) {
          LOG_ERR("Unable to open output file");
          return;
        }
        if (rtlfile.write(rtlbuffer) != rtlbuffer.size()) {
          LOG_ERR("RTL file write error");
        }
      }
    }
    else {
      QString host;
      if (item == "LAOS") {
        host = "x.x.x.x";
        this->laosclient->print(host, "MyDocument", rtlbuffer);
      }
      else {
        if (item == "Lazzzor") host = "10.20.30.27";
        else host = "localhost";
        this->lpdclient->print(host, "MyDocument", rtlbuffer);
      }
    }
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
  QRectF brect = this->documentitem->boundingRect();
  qDebug() << "brect: " << brect.topLeft().x() << "," << brect.topLeft().y();
  QPointF topleft = brect.topLeft();
  this->documentitem->setPos(-topleft);
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
