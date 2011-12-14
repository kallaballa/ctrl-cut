#include "MainWindow.h"
#include <QtGui>
#include "LpdClient.h"

#include "FileParser.h"
#include "Document.h"
#include "cut/model/CutModel.h"
#include "cut/geom/Geometry.h"
#include "svg/Svg2Ps.h"

#include "StreamUtils.h"
#include "GroupItem.h"
#include "CtrlCutScene.h"

#include "LaserDialog.h"

#include <assert.h>
#include <boost/thread.hpp>
MainWindow *MainWindow::inst = NULL;

MainWindow::MainWindow() : document(NULL), laserdialog(NULL)
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
    if (this->document) {
      delete this->document;
    }
    this->document = new Document();
    this->document->load(filename.toStdString());

    if (!document->engraveList.empty()) {
      this->rasterpixmap = QPixmap();
      this->rasterpos = QPointF();
      AbstractImage *image = document->front_engrave()->sourceImage();
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

  this->documentitem = new QGraphicsItemGroup();
  this->documentitem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
  this->scene->addItem(this->documentitem);

  if (!this->document->cutList.empty()) {
    this->firstitem = NULL;
    for (CutModel::iterator iter = this->document->front_cut()->begin(); iter != this->document->front_cut()->end(); iter++) {
      const Segment &segment = **iter;
      QGraphicsLineItem *line =
        new QGraphicsLineItem(segment[0][0], segment[0][1], segment[1][0], segment[1][1],
                              this->documentitem);
      this->documentitem->addToGroup(line);
      if (!this->firstitem) this->firstitem = line;
    }
  }

  if (!this->rasterpixmap.isNull()) {
    this->rasteritem = new QGraphicsPixmapItem(this->rasterpixmap, this->documentitem);
    this->rasteritem->setPos(this->rasterpos);
    this->documentitem->addToGroup(this->rasteritem);
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
  if (!this->document) {
    fprintf(stderr, "No document loaded\n");
    return;
  }

  if (!this->laserdialog) this->laserdialog = new LaserDialog(this);
  if (this->laserdialog->exec() != QDialog::Accepted) return;

  this->laserdialog->updateLaserConfig(*this->document);

  QStringList items;
  items << "Lazzzor" << "localhost";
  bool ok;
  QString item = QInputDialog::getItem(this, "Send to where?", "Send to where?", items, 0, false, &ok);
  if (ok && !item.isEmpty()) {
    QString host = (item == "Lazzzor")?"10.20.30.27":"localhost";


    // Apply transformations and add to job
    QPointF pos = this->documentitem->pos();
/* REFACTOR
    if (this->cutmodel) {
      //      this->cutmodel->setTransformation(pos); // Assumes initial translation to be (0,0)
      job.addCut(this->cutmodel);
    }
    QPointF rasterpos = this->rasteritem->pos() + pos;
    if (this->raster) {
      this->raster->sourceImage()->setTranslation(rasterpos.x(), rasterpos.y());
      job.addRaster(this->raster);
    }
    */

    QByteArray rtlbuffer;
    ByteArrayOStreambuf streambuf(rtlbuffer);
    std::ostream ostream(&streambuf);
    this->document->preprocess();
    this->document->write(ostream);
    
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
