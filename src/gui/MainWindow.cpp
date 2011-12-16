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
    Document& loaded = * new Document();
    loaded.load(filename.toStdString());
    this->documentitem = new DocumentItem(*this->scene,loaded);
  }
}

void MainWindow::importFile(const QString &filename)
{
  if (!filename.isEmpty()) {
    Document& loaded = * new Document();
    loaded.load(filename.toStdString());
    if(this->documentitem == NULL) {
      this->documentitem = new DocumentItem(*this->scene,loaded);
    } else {
      this->documentitem->load(loaded);
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
  this->documentitem->doc.settings.put(DocumentSettings::USER, "DefaultUser");
  QStringList items;
  items << "Lazzzor" << "localhost";
  bool ok;
  QString item = QInputDialog::getItem(this, "Send to where?", "Send to where?", items, 0, false, &ok);
  if (ok && !item.isEmpty()) {
    QString host = (item == "Lazzzor")?"10.20.30.27":"localhost";
    foreach (QGraphicsItem *sitem, this->scene->items()) {
      CutItem* ci = NULL;
      EngraveItem* ei = NULL;
      QPointF pos;
      if((ci = dynamic_cast<CutItem*>(sitem))) {
        pos = ci->pos();
        ci->cut.setTranslation(* new Point(pos.x(), pos.y()));
      } else if((ei = dynamic_cast<EngraveItem*>(sitem))) {
/* REFACTOR
       pos = ei->pos();
        ei->engraving.setTranslation(new Point(pos.x(), pos.y()));
        */
      }
    }

/* REFACTOR
    // Apply transformations
    QPointF pos = this->documentitem->pos();

    if (!this->documentitem.doc.empty_cut()) {
      this->document->front_cut()->setTranslation(* new Point(pos.x(), pos.y()));
    }
    QPointF rasterpos = this->rasteritem->pos() + pos;
    if (!this->document->empty_engrave()) {
      this->document->front_engrave()->sourceImage()->setTranslation(rasterpos.x(), rasterpos.y());
    }

*/
    QByteArray rtlbuffer;
    ByteArrayOStreambuf streambuf(rtlbuffer);
    std::ostream ostream(&streambuf);
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
