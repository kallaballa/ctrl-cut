/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include <QAbstractSocket>
#include <QtGui>
#include <QUndoView>
#include <QMessageBox>
#include <QFileDialog>
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
#include "PreviewDialog.hpp"
#include <svg/SvgWriter.hpp>
#include "SendDialog.hpp"
#include <algorithm>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "helpers/Qt.hpp"

MainWindow *MainWindow::inst = NULL;

MainWindow::MainWindow() : laserdialog(NULL), simdialog(NULL) {

  loadGuiConfig();
  this->undoStack = new QUndoStack(this);
  this->undoStack->setObjectName("undoStack");
  this->lpdclient = new LpdClient(this);
  this->lpdclient->setObjectName("lpdclient");

  createUndoView();

  setupUi(this);

  createActions();
  this->scene = new CtrlCutScene(this);
  // this->objectProperties->setDocument(this->scene->getDocumentHolder()->doc);
 // this->objectProperties->disable();
  this->graphicsView->setScene(this->scene);
  this->graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);

  sendProgressDialog.setWindowFlags(Qt::Dialog | Qt::Desktop);
  sendProgressDialog.setWindowModality(Qt::ApplicationModal);

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

  QObject::connect(center, SIGNAL(currentIndexChanged(int)),
      objectProperties, SLOT(on_center_update(int)));

  QObject::connect(airAssistBox, SIGNAL(currentIndexChanged(int)),
      objectProperties, SLOT(on_airAssist_update(int)));

  QObject::connect(unit, SIGNAL(currentIndexChanged(int)),
      objectProperties, SLOT(on_unit_update(int)));

  QObject::connect(titleEdit, SIGNAL(textEdited(const QString&)),
      objectProperties, SLOT(on_title_update(const QString&)));

  QObject::connect(autofocusBox, SIGNAL(stateChanged(int)),
      objectProperties, SLOT(on_autofocus_update(int)));

  QObject::connect(reduceEdit, SIGNAL(textEdited(QString)),
      objectProperties, SLOT(on_reduce_update(const QString&)));

  QObject::connect(quitAction, SIGNAL(triggered()),
      QApplication::instance(), SLOT(quit()));

  QObject::connect(mergeAction, SIGNAL(triggered()),
      this, SLOT(on_toolsMerge()));

  QObject::connect(selectAllAction, SIGNAL(triggered()),
      this, SLOT(on_selectAll()));

  QObject::connect(sendToFileAction, SIGNAL(triggered()),
      this, SLOT(on_extraSendToFileAction_triggered()));

  QObject::connect(toolsPreviewAction, SIGNAL(triggered()),
      this, SLOT(on_previewAction_triggered()));

  QObject::connect(fileSendAction, SIGNAL(triggered()),
      this, SLOT(on_filePrintAction_triggered()));

  QObject::connect(&sendProgressDialog, SIGNAL(canceled()), this->lpdclient, SLOT(on_cancel()));

  this->editCopySettingsAction->setEnabled(false);
  this->editPasteSettingsAction->setEnabled(false);
  this->mergeAction->setEnabled(false);
  createContextMenu();
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
  menu->addAction(editCopyAction);
  menu->addAction(editPasteAction);
  menu->addAction(editCopySettingsAction);
  menu->addAction(editPasteSettingsAction);
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

void MainWindow::on_editCopySettingsAction_triggered() {
  assert(this->scene->selectedItems().size() == 1);
  this->scene->settingsClipboard.clear();
  QGraphicsItem *item = this->scene->selectedItems()[0];

  if (CutItem *ci = dynamic_cast<CutItem *>(item)) {
    this->scene->settingsClipboard.append(&ci->cut->settings);
  }
  if (EngraveItem *ei = dynamic_cast<EngraveItem *>(item)) {
    this->scene->settingsClipboard.append(&ei->engraving->settings);
  }

  this->editPasteSettingsAction->setEnabled(!this->scene->settingsClipboard.empty());
}

void MainWindow::on_editPasteAction_triggered() {
  PasteCommand *pasteCommand = new PasteCommand(this->scene);
  undoStack->push(pasteCommand);
}

void MainWindow::on_editPasteSettingsAction_triggered() {
  PasteSettingsCommand *pasteSettings = new PasteSettingsCommand(this->scene);
  undoStack->push(pasteSettings);
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

void MainWindow::on_previewAction_triggered() {
  PreviewDialog pd(*this->scene->getDocumentHolder()->doc);
  pd.exec();
}

void MainWindow::on_fileNewAction_triggered() {

  if (!maybeSave()) return;
  NewDialog nd;
  nd.loadFrom(this->guiConfig);
  if (nd.exec() == QDialog::Accepted) {
    this->scene->newJob(nd.getResolution(),nd.getWidth(),nd.getHeight());
    nd.saveTo(this->guiConfig);
    setWindowTitle("untitled.cut");
    setWindowFilePath("");
    this->undoStack->setClean();
    this->scene->getDocumentHolder()->filename = "";
    this->objectProperties->setDocument(this->scene->getDocumentHolder()->doc);
  }

}

void MainWindow::openFile(const QString &filename) {
  if(filename.isNull())
    return;

  this->scene->open(filename);
  this->scene->getDocumentHolder()->filename = filename;
  this->objectProperties->setDocument(this->scene->getDocumentHolder()->doc);
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
  QString f = filename;
  if (f.isEmpty()) return;
  if(!f.endsWith(".cut", Qt::CaseInsensitive))
    f += ".cut";

  typedef DocumentSettings DS;
  Document doc = *this->scene->getDocumentHolder()->doc.get();
  if(doc.get(DS::TITLE) == "untitled")
    doc.put(DS::TITLE, QFileInfo(f).baseName().toStdString());
  std::ofstream os(f.toStdString());
  SvgWriter svgW(doc, os);
  svgW.write(doc);
  this->scene->getDocumentHolder()->filename = f;
  setWindowTitle("");
  setWindowFilePath(f);
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
  importFile(QFileDialog::getOpenFileName(this, "Import File", "", "Supported files (*.ps *.eps *.pdf *.vector *.svg)"));
}

void MainWindow::on_filePrintAction_triggered()
{
  SendDialog sd;
  sd.loadFrom(guiConfig);
  if (sd.exec()) {
    QString host = sd.getNetworkAddress();
    sd.saveTo(guiConfig);
    QByteArray rtlbuffer;
    ByteArrayOStreambuf streambuf(rtlbuffer);
    std::ofstream tmpfile("gui.tmp", std::ios::out | std::ios::binary);
    std::ostream ostream(&streambuf);

    //make a copy and run optimization
    Document doc = *this->scene->getDocumentHolder()->doc;
    //couldn't merge all cuts
    if(doc.cuts().size() > 1) {
      bool overlap = false;
      for(CutPtr cut1: doc.cuts()) {
        for(CutPtr cut2: doc.cuts()) {
          if(cut1 == cut2 || cut1->settings == cut2->settings)
            continue;

          Box b1 = find_bounding_box(*cut1.get());
          Box b2 = find_bounding_box(*cut2.get());

          overlap = intersects(b1, b2) || intersects(b2, b1);
          if(overlap)
            break;
        }
      }

      if(overlap) {
        if(showYesNoDialog("Warning!", "You have overlapping cuts with different settings. That might lead to undesired behavior. Do you want to continue?","The two cuts will be executed in two different passes and optimization will also be applied separately. That might break the correctness of the inner-outer sorting.") == QMessageBox::No) {
          return;
        }
      }
    }

    doc.mergeCuts();
    doc.optimize();

    EpilogLegend36Ext cutter;
    cutter.write(doc,ostream);
    tmpfile.flush();


    sendProgressDialog.show();
    this->lpdclient->print(host, "MyDocument", rtlbuffer);
  }
}

void MainWindow::on_extraSendToFileAction_triggered()
{
  QString filename = QFileDialog::getSaveFileName(this, "Save File", "", "Raw format (*.raw)");
  if(filename.isEmpty())
    return;

  if(!filename.endsWith(".raw", Qt::CaseInsensitive))
    filename += ".raw";

  QByteArray rtlbuffer;
  std::ofstream os(filename.toStdString(), std::ios::out | std::ios::binary);

  //make a copy and run optimization
  Document doc = *this->scene->getDocumentHolder()->doc;

  doc.mergeCuts();
  doc.optimize();

  EpilogLegend36Ext cutter;
  cutter.write(doc,os);
}

void MainWindow::on_lpdclient_done(bool error)
{
  if (error) fprintf(stderr, "LPD error\n");
  else printf("LPD done\n");
  sendProgressDialog.hide();
}

void MainWindow::on_lpdclient_progress(int done, int total)
{
  sendProgressDialog.setValue(100.0f*done/total);
  printf("Progress: %.0f%%\n", 100.0f*done/total);
}

void MainWindow::sceneSelectionChanged()
{
  printf("selectionChanged\n");


  QList<QGraphicsItem *> selecteditems = this->scene->selectedItems();
  foreach(QGraphicsItem *item, this->scene->items()) {
    if (AbstractCtrlCutItem *cci = dynamic_cast<AbstractCtrlCutItem*>(item)) {
      cci->setHighlighted(false);
    }
  }

  foreach(QGraphicsItem *item, selecteditems) {
    if (AbstractCtrlCutItem *cci = dynamic_cast<AbstractCtrlCutItem*>(item)) {
      cci->setHighlighted(true);
    }
  }

  /*
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
*/
  if (selecteditems.empty()) {
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

    if(selecteditems.size() > 1) {
      this->objectProperties->disable();
      bool allCutItems = true;
      foreach (QGraphicsItem *item, selecteditems) {
        if(dynamic_cast<AbstractCtrlCutItem*>(item) != NULL && dynamic_cast<CutItem*>(item) == NULL) {
          allCutItems = false;
        }
      }
      this->mergeAction->setEnabled(allCutItems);
    } else {
      foreach (QGraphicsItem *item, this->scene->items()) {
        if (item->parentItem()) continue;
        if (AbstractCtrlCutItem *cci = dynamic_cast<AbstractCtrlCutItem*>(item)) {
          if(item->isSelected()) {
            this->objectProperties->enable(cci);
          }
        }
      }
    }

    if(selecteditems.size() == 1) {
      this->editCopySettingsAction->setEnabled(true);
    }
    else {
      this->editCopySettingsAction->setEnabled(false);
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
                                   " it under the terms of the GNU General Public License as published by"
                                   " the Free Software Foundation; either version 2 of the License, or"
                                   " (at your option) any later version."));
}

void MainWindow::on_itemMoved(QGraphicsItem *movedItem,
                           const QPointF &oldPosition) {
    undoStack->push(new MoveCommand(this->scene, movedItem, oldPosition));
}

void MainWindow::on_windowShowPropertiesAction_triggered()
{
  if (windowShowPropertiesAction->isChecked()) {
    this->propertiesDockWidget->show();
  } else {
    this->propertiesDockWidget->close();
  }
}

void MainWindow::on_propertiesDockWidget_visibilityChanged(bool visible)
{
  windowShowPropertiesAction->setChecked(visible);
}

void MainWindow::on_undoStack_cleanChanged(bool clean)
{
}

void MainWindow::on_toolsMerge() {
  MergeCommand* m = new MergeCommand(this->scene);
  undoStack->push(m);
}

void MainWindow::on_selectAll() {
  AbstractCtrlCutItem *cci;
  foreach(QGraphicsItem* item, this->scene->items()) {
    if((cci = dynamic_cast<AbstractCtrlCutItem* >(item))) {
      item->setSelected(true);
    }
  }
}

void MainWindow::saveGuiConfig() {
  using boost::property_tree::ptree;

  QDir dir(QDir::homePath() + QDir::separator() + ".ctrl-cut");
  if (!dir.exists()) {
      dir.mkpath(".");
  }
  string filename = (dir.path() + QDir::separator()).toStdString() + "config";
  try {
    std::ofstream of(filename);

    ptree config;
    config.put("Driver",guiConfig.driver);
    config.put("Unit",guiConfig.unit);
    config.put("Resolution",guiConfig.resolution);
    config.put("BedWidth",guiConfig.bedWidth);
    config.put("BedHeight",guiConfig.bedHeight);
    config.put("NetworkAddress",guiConfig.networkAddress);
    config.put("Version",TOSTRING(CTRLCUT_VERSION));

    write_ini( of, config);
  } catch(std::exception& ex) {
    showErrorDialog("Unable to save startup config: " + filename, ex.what());
  }
}

void MainWindow::loadGuiConfig() {
  using boost::property_tree::ptree;
  ptree config;

  QFile file(QDir::homePath() + QDir::separator() + ".ctrl-cut" + QDir::separator() + "config");
  string filename = (QDir::homePath() + QDir::separator() + ".ctrl-cut" + QDir::separator() + "config").toStdString();
  if (file.exists()) {
    try {
      std::ifstream is(filename);
      read_ini(is, config);
      guiConfig.driver = (LaserCutter::Driver) config.get<int>("Driver");
      guiConfig.unit = (Unit) config.get<int>("Unit");
      guiConfig.resolution = config.get<size_t>("Resolution");
      guiConfig.bedWidth = config.get<double>("BedWidth");
      guiConfig.bedHeight = config.get<double>("BedHeight");
      guiConfig.networkAddress = config.get<string>("NetworkAddress");
      guiConfig.version = config.get<string>("Version");
    } catch(std::exception& ex) {
       showErrorDialog("Unable to load startup config: " + filename, ex.what());
    }
  }

}
