#include "MainWindow.h"

MainWindow *MainWindow::inst = NULL;

MainWindow::MainWindow() : rawDocItem(NULL), processDocItem(NULL), laserdialog(NULL), simdialog(NULL)
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
    if(this->rawDocItem != NULL) {
      delete this->rawDocItem;
    }
    Document& loading = * new Document();
    loading.put(EngraveSettings::DITHERING, EngraveSettings::BAYER);
    loading.put(DocumentSettings::LOAD_ENGRAVING, true);
    loading.load(filename.toStdString());
    this->rawDocItem = new DocumentItem(*this->scene,loading);
  }
}

void MainWindow::importFile(const QString &filename)
{
  if (!filename.isEmpty()) {
    Document& loading = * new Document();
    loading.put(EngraveSettings::DITHERING, EngraveSettings::BAYER);
    loading.put(DocumentSettings::LOAD_ENGRAVING, true);

    loading.load(filename.toStdString());
    if(this->rawDocItem == NULL) {
      this->rawDocItem = new DocumentItem(*this->scene,loading);
    } else {
      this->rawDocItem->load(loading);
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
  if (!this->rawDocItem) {
    fprintf(stderr, "No document loaded\n");
    return;
  }

  if (!this->laserdialog) this->laserdialog = new LaserDialog(this);
  if (this->laserdialog->exec() != QDialog::Accepted) return;

  this->laserdialog->updateLaserConfig(this->rawDocItem->doc);
  this->rawDocItem->doc.put(DocumentSettings::TITLE, "Default Title");
  this->rawDocItem->doc.put(DocumentSettings::USER, "Default User");
 /* QStringList items;
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
  }*/
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


void MainWindow::simulate() {
  if (!this->rawDocItem) {
    fprintf(stderr, "No document loaded\n");
    return;
  }

  if (!this->simdialog) this->simdialog = new SimulatorDialog(*this->rawDocItem,this);
  this->simdialog->setVisible(true);
}
