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

#include <QAbstractSocket>
#include <QtGui>
#include <assert.h>
#include "LpdClient.hpp"
#include "StreamUtils.hpp"
#include "GroupItem.hpp"
#include "helpers/GraphicsItems.hpp"
#include "cutters/EpilogLegend36Ext.hpp"
#include "MainWindow.hpp"
#include "Document.hpp"
#include "CtrlCutScene.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include <qapplication.h>
#include "Commands.hpp"
#include <QGraphicsItem>
#include "NewDialog.hpp"
#include <algorithm>

MainWindow *MainWindow::inst = NULL;

MainWindow::MainWindow() : laserdialog(NULL), simdialog(NULL) {

  this->undoStack = new QUndoStack(this);
  this->undoStack->setObjectName("undoStack");
  this->lpdclient = new LpdClient(this);
  this->lpdclient->setObjectName("lpdclient");

  createUndoView();
  createContextMenu();

  setupUi(this);

  createActions();

  this->scene = new CtrlCutScene(this);
  this->graphicsView->setScene(this->scene);
  this->graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);

  QObject::connect(graphicsView, SIGNAL(customContextMenuRequested(const QPoint&)),
      this, SLOT(showContextMenu(const QPoint&)));

  QObject::connect(scene, SIGNAL(selectionChanged()), this,
      SLOT(sceneSelectionChanged()));
  QObject::connect(scene, SIGNAL(sceneRectChanged(const QRectF&)),
      this->graphicsView, SLOT(updateSceneRect(const QRectF&)));

  QObject::connect(graphicsView, SIGNAL(fileDropped(const QString &)), this,
      SLOT(openFile(const QString &)));

  QObject::connect(scene, SIGNAL(itemMoved(QGraphicsItem*,QPointF)), this,
      SLOT(on_itemMoved(QGraphicsItem*,QPointF)));

  QObject::connect(power, SIGNAL(valueChanged(QString)),
      objectProperties, SLOT(on_power_update(const QString&)));

  QObject::connect(speed, SIGNAL(valueChanged(QString)),
      objectProperties, SLOT(on_speed_update(const QString&)));

  QObject::connect(frequency, SIGNAL(valueChanged(QString)),
      objectProperties, SLOT(on_frequency_update(const QString&)));

  QObject::connect(posX, SIGNAL(textEdited(QString)),
      objectProperties, SLOT(on_posX_update(const QString&)));

  QObject::connect(posY, SIGNAL(textEdited(QString)),
      objectProperties, SLOT(on_posY_update(const QString&)));

  QObject::connect(sort, SIGNAL(currentIndexChanged(int)),
      objectProperties, SLOT(on_sort_update(int)));

  QObject::connect(direction, SIGNAL(currentIndexChanged(int)),
      objectProperties, SLOT(on_direction_update(int)));

  QObject::connect(dithering, SIGNAL(currentIndexChanged(int)),
      objectProperties, SLOT(on_dithering_update(int)));

  QObject::connect(unit, SIGNAL(currentIndexChanged(int)),
      objectProperties, SLOT(on_unit_update(int)));
}

MainWindow::~MainWindow()
{}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (maybeSave()) event->accept();
  else event->ignore();
}

void MainWindow::showContextMenu(const QPoint& pos) {
  QPoint globalPos = this->scene->views()[0]->mapToGlobal(pos);
  if(this->scene->selectedItems().size() != 1) {
    lowerAct->setEnabled(false);
    raiseAct->setEnabled(false);
    bottomAct->setEnabled(false);
    topAct->setEnabled(false);
  } else {
    lowerAct->setEnabled(true);
    raiseAct->setEnabled(true);
    bottomAct->setEnabled(true);
    topAct->setEnabled(true);
  }
  menu->exec(globalPos);
}

void MainWindow::createContextMenu() {
  this->menu = new QMenu();

  this->lowerAct = new QAction(tr("&Lower"), this);
  this->lowerAct->setStatusTip(tr("Lower an item"));
  this->raiseAct = new QAction(tr("&Raise"), this);
  this->raiseAct->setStatusTip(tr("Raise an item"));
  this->bottomAct = new QAction(tr("&Lower to bottom"), this);
  this->bottomAct->setStatusTip(tr("Lower an item to the bottom"));
  this->topAct = new QAction(tr("&Raise to top"), this);
  this->topAct->setStatusTip(tr("Raise an item to the top"));

  connect(lowerAct, SIGNAL(triggered()), this, SLOT(on_lowerItem()));
  connect(raiseAct, SIGNAL(triggered()), this, SLOT(on_raiseItem()));
  connect(bottomAct, SIGNAL(triggered()), this, SLOT(on_lowerItemToBottom()));
  connect(topAct, SIGNAL(triggered()), this, SLOT(on_raiseItemToTop()));

  menu->addAction(lowerAct);
  menu->addAction(raiseAct);
  menu->addAction(bottomAct);
  menu->addAction(topAct);
}

void MainWindow::createUndoView(){
  return;
    undoView = new QUndoView(undoStack);
    undoView->setWindowTitle(tr("Command List"));
    undoView->show();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
}

void MainWindow::createActions() {
    undoAction = undoStack->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    this->editMenu->addAction(undoAction);

    redoAction = undoStack->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);
    this->editMenu->addAction(redoAction);

    // Macs usually don't have a Delete key, use Backspace instead
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::Key_Backspace));
    shortcuts.append(QKeySequence::Delete);
    this->editCutAction->setShortcuts(shortcuts);
}

void MainWindow::on_lowerItem() {
  QUndoCommand *liCmd = new LowerItemCommand(this->scene);
  undoStack->push(liCmd);
}

void MainWindow::on_raiseItem() {
  QUndoCommand *riCmd = new RaiseItemCommand(this->scene);
  undoStack->push(riCmd);
}

void MainWindow::on_lowerItemToBottom() {
  QUndoCommand *libCmd = new LowerItemToBottomCommand(this->scene);
  undoStack->push(libCmd);
}

void MainWindow::on_raiseItemToTop() {
  QUndoCommand *ritCmd = new RaiseItemToTopCommand(this->scene);
  undoStack->push(ritCmd);
}

void MainWindow::on_editCutAction_triggered() {
  if (this->scene->selectedItems().isEmpty()) return;

  QUndoCommand *deleteCommand = new DeleteCommand(this->scene);
  undoStack->push(deleteCommand);
}

void MainWindow::on_editCopyAction_triggered() {
  this->scene->itemClipboard.clear();
  foreach (QGraphicsItem *item, this->scene->selectedItems()) {
    if (AbstractCtrlCutItem *ci = dynamic_cast<AbstractCtrlCutItem *>(item)) {
      this->scene->itemClipboard.append(ci);
    }
  }
  this->editPasteAction->setEnabled(!this->scene->itemClipboard.isEmpty());
}

void MainWindow::on_editPasteAction_triggered() {
  QUndoCommand *pasteCommand = new PasteCommand(this->scene);
  undoStack->push(pasteCommand);
}

void MainWindow::on_editGroupAction_triggered() {
  if (this->scene->selectedItems().length() <= 1) return;

  undoStack->push(new GroupCommand(this->scene));
}

void MainWindow::on_editUngroupAction_triggered() {
  if (this->scene->selectedItems().length() != 1) return;

  undoStack->push(new UnGroupCommand(this->scene));
}

bool MainWindow::maybeSave()
{
  if (this->undoStack->isClean()) return true;
    
  QMessageBox::StandardButton ret;
  QMessageBox box(this);
  box.setText("The document has been modified.");
  box.setInformativeText("Do you want to save your changes?");
  box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  box.setDefaultButton(QMessageBox::Save);
  box.setIcon(QMessageBox::Warning);
  box.setWindowModality(Qt::ApplicationModal);
#ifdef Q_OS_MAC
  // Cmd-D is the standard shortcut for this button on Mac
  box.button(QMessageBox::Discard)->setShortcut(QKeySequence("Ctrl+D"));
  box.button(QMessageBox::Discard)->setShortcutEnabled(true);
#endif
  ret = (QMessageBox::StandardButton) box.exec();
  
  if (ret == QMessageBox::Save) {
    on_fileSaveAction_triggered();
    // Returns false on failed save
    return this->undoStack->isClean();
  }
  else if (ret == QMessageBox::Cancel) {
    return false;
  }
  return true;
}

void MainWindow::on_fileNewAction_triggered() {

  if (!maybeSave()) return;

  NewDialog nd;
  if (nd.exec() == QDialog::Accepted) {
    int resolution = nd.getResolution();
    this->scene->newJob(resolution,
                        Distance(36, IN, resolution), 
                        Distance(24, IN, resolution));
    setWindowTitle("untitled.cut");
    setWindowFilePath("");
    this->undoStack->setClean();
    this->scene->getDocumentHolder()->filename = "";
  }
}

void MainWindow::openFile(const QString &filename) {
  if(filename.isNull())
    return;

  QUndoCommand *openCommand = new OpenCommand(this->scene, filename);
  undoStack->push(openCommand);
  setWindowTitle("");
  setWindowFilePath(filename);
  undoStack->setClean();
}

void MainWindow::importFile(const QString &filename) {
  if(filename.isNull())
    return;

  QUndoCommand *importCommand = new ImportCommand(this->scene, filename);
  undoStack->push(importCommand);
}

void MainWindow::saveFile(const QString &filename) {
  if (filename.isEmpty()) return;

  SaveCommand *saveCommand = new SaveCommand(this->scene, filename);
  //We don't need undo for save commands, but let's keep the command pattern anyway
  //undoStack->push(saveCommand);
  saveCommand->modify();
  setWindowTitle("");
  setWindowFilePath(filename);
  undoStack->setClean();
}

void MainWindow::on_fileOpenAction_triggered()
{
  if (!maybeSave()) return;
  openFile(QFileDialog::getOpenFileName(this, "Open File", "", "Ctrl-Cut Document (*.cut)"));
}

void MainWindow::on_fileSaveAction_triggered()
{
  QString filename = this->scene->getDocumentHolder()->filename;
  if (filename.isEmpty()) {
    filename = QFileDialog::getSaveFileName(this, "Save File", "", "Ctrl-Cut Document (*.cut)");
  }

  saveFile(filename);
}

void MainWindow::on_fileSaveAsAction_triggered()
{
  saveFile(QFileDialog::getSaveFileName(this, "Save File", "", "Ctrl-Cut Document (*.cut)"));
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
    this->scene->getDocumentHolder()->doc->put(DocumentSettings::TITLE, "title");
    this->scene->getDocumentHolder()->doc->put(DocumentSettings::USER, "user");
//    this->scene->getDocumentHolder().doc->write(tmpfile);

    //make a copy and run optimization
    Document doc = *this->scene->getDocumentHolder()->doc;
    for(CutPtr c : doc.cuts()) {
      c->normalize();
      c->sort();
      c->translate();
    }
    EpilogLegend36Ext cutter;
    cutter.write(doc,ostream);
    ostream << std::endl;
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

  QList<QGraphicsItem *> selecteditems = this->scene->selectedItems();

  foreach(QGraphicsItem *item, selecteditems) {
    QRectF rect = item->boundingRect();
    QRectF chrect = item->childrenBoundingRect();
    printf("%p: (%.0f,%.0f) [%.0f,%.0f,%.0f,%.0f]  [%.0f,%.0f,%.0f,%.0f]\n", item, item->x(), item->y(), 
           rect.x(), rect.y(), rect.width(), rect.height(),
           chrect.x(), chrect.y(), chrect.width(), chrect.height());
    if (CtrlCutGroupItem *gi = dynamic_cast<CtrlCutGroupItem*>(item)) {
      foreach(QGraphicsItem *gci, gi->childItems()) {
        printf("  -%p: (%.0f,%.0f)  [%.0f,%.0f,%.0f,%.0f]\n", gci, gci->x(), gci->y(),
               gci->boundingRect().x(), gci->boundingRect().y(), gci->boundingRect().width(), gci->boundingRect().height());
      }
    }
  }

  if (selecteditems.empty()) {
    foreach (QGraphicsItem *item, this->scene->items()) {
      if (AbstractCtrlCutItem *cci = dynamic_cast<AbstractCtrlCutItem*>(item)) {
        //        cci->setHighlighted(false);
      }
    }
    this->objectProperties->disable();
  } else {

    CtrlCutGroupItem *gi;
    if (selecteditems.length() == 1 &&
        (gi = dynamic_cast<CtrlCutGroupItem*>(selecteditems[0]))) {
      this->editUngroupAction->setEnabled(true);
    }
    else {
      this->editUngroupAction->setEnabled(false);
    }

    foreach (QGraphicsItem *item, this->scene->items()) {
      if (item->parentItem()) continue;
      if (AbstractCtrlCutItem *cci = dynamic_cast<AbstractCtrlCutItem*>(item)) {
        if(item->isSelected()) {
          //          cci->setHighlighted(true);
          this->objectProperties->enable(cci);
        } else {
          //          cci->setHighlighted(false);
        }
      }
    }
  }
}

void MainWindow::on_toolsMoveToOriginAction_triggered()
{
  QUndoCommand *moveToOrigin = new MoveToOriginCommand(this->scene);
  undoStack->push(moveToOrigin);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

void
MainWindow::on_helpAboutAction_triggered()
{
  qApp->setWindowIcon(QApplication::windowIcon());
  QMessageBox::information(this, "About Ctrl-Cut",
                           QString("Ctrl-Cut " TOSTRING(CTRLCUT_VERSION) " (http://github.com/metalab/ctrl-cut)\n") +
                           QString("Copyright (C) 2009-2014 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>\n"
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

void MainWindow::on_windowShowPropertiesAction_triggered()
{
  if (windowShowPropertiesAction->isChecked()) {
    this->objectProperties->hide();
  } else {
    this->objectProperties->show();
  }
}

void MainWindow::on_undoStack_cleanChanged(bool clean)
{
}
