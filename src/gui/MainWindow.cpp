/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "MainWindow.hpp"
#include "Document.hpp"
#include "CtrlCutScene.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include <qapplication.h>
#include "Commands.hpp"
#include <QGraphicsItem>
#include "delegates/DistanceDelegate.hpp"

MainWindow *MainWindow::inst = NULL;

MainWindow::MainWindow() : laserdialog(NULL), simdialog(NULL) {
  this->undoStack = new QUndoStack(this);
  this->lpdclient = new LpdClient(this);
  this->lpdclient->setObjectName("lpdclient");

  createActions();
  createUndoView();

  setupUi(this);

  this->scene = new CtrlCutScene(this);
  this->graphicsView->setScene(this->scene);

  connect(this->scene, SIGNAL(selectionChanged()), this,
      SLOT(sceneSelectionChanged()));
  connect(this->scene, SIGNAL(sceneRectChanged(const QRectF&)),
      this->graphicsView, SLOT(updateSceneRect(const QRectF&)));

  connect(this->graphicsView, SIGNAL(fileDropped(const QString &)), this,
      SLOT(openFile(const QString &)));

  connect(scene, SIGNAL(itemMoved(QGraphicsItem*,QPointF)), this,
      SLOT(on_itemMoved(QGraphicsItem*,QPointF)));
}

MainWindow::~MainWindow()
{}

void MainWindow::createUndoView(){
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void MainWindow::createActions() {
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);
}

void MainWindow::on_deleteItem() {
  if (this->scene->selectedItems().isEmpty())
    return;

  QUndoCommand *deleteCommand = new DeleteCommand(this->scene);
  undoStack->push(deleteCommand);
}

void MainWindow::on_newJob() {
  QUndoCommand *newCommand = new NewCommand(this->scene);
  undoStack->push(newCommand);
  setWindowTitle("Ctrl-Cut - " + QString(this->scene->getDocumentHolder().doc->get(DocumentSettings::TITLE).c_str()));
}

/*
void MainWindow::on_simplifyItem() {
  if (this->scene->selectedItems().isEmpty())
    return;

  QUndoCommand *simplifyCommand = new SimplifyCommand(this->scene);
  undoStack->push(simplifyCommand);
}
*/
void MainWindow::openFile(const QString &filename) {
  QUndoCommand *openCommand = new OpenCommand(this->scene, filename);
  undoStack->push(openCommand);
}

void MainWindow::importFile(const QString &filename) {
  QUndoCommand *importCommand = new ImportCommand(this->scene, filename);
  undoStack->push(importCommand);
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
  /*
  if (!this->laserdialog) this->laserdialog = new LaserDialog(this);
  if (this->laserdialog->exec() != QDialog::Accepted) return;

  this->laserdialog->updateLaserConfig(*this->scene->getDocumentHolder().doc);
*/
 QStringList items;
  items << "Lazzzor" << "localhost";
  bool ok;
  QString item = QInputDialog::getItem(this, "Send to where?", "Send to where?",
      items, 0, false, &ok);
  if (ok && !item.isEmpty()) {
    QString host = (item == "Lazzzor") ? "10.20.30.27" : "localhost";

    QByteArray rtlbuffer;
    ByteArrayOStreambuf streambuf(rtlbuffer);
    std::ofstream tmpfile("gui.tmp", std::ios::out | std::ios::binary);
    std::ostream ostream(&streambuf);
    this->scene->getDocumentHolder().doc->put(DocumentSettings::TITLE, "title");
    this->scene->getDocumentHolder().doc->put(DocumentSettings::USER, "user");
//    this->scene->getDocumentHolder().doc->write(tmpfile);

    this->scene->getDocumentHolder().doc->write(ostream);
    tmpfile.close();

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

  if(this->scene->selectedItems().empty()) {
    foreach (QGraphicsItem *item, this->scene->items()) {
      AbstractCtrlCutItem* cci;
      if((cci = dynamic_cast<AbstractCtrlCutItem*>(item)))
        cci->setHighlighted(false);
    }
    this->objectProperties->hide();
  } else {
    foreach (QGraphicsItem *item, this->scene->items()) {
      EngraveItem* ei;
      CutItem* ci;

      if(item->isSelected()) {
        if((ei = dynamic_cast<EngraveItem*>(item))) {
          ei->setHighlighted(true);
          this->objectProperties->show(ei);
        } else if((ci = dynamic_cast<CutItem*>(item))) {
          ci->setHighlighted(true);
          this->objectProperties->show(ci);
        }
      } else {
        AbstractCtrlCutItem* cci;
        if((cci = dynamic_cast<AbstractCtrlCutItem*>(item))) {
          cci->setHighlighted(false);
        }
      }
      printf("item: %p\n", item);
    }
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

void MainWindow::on_itemMoved(QGraphicsItem *movedItem,
                           const QPointF &oldPosition) {
    undoStack->push(new MoveCommand(this->scene, movedItem, oldPosition));
}
