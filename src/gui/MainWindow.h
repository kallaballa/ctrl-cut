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
#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_


#include <QMainWindow>
#include "ui_CtrlCut.h"
#include <QAbstractSocket>
#include <QtGui>
#include <assert.h>
#include "LpdClient.h"
#include "StreamUtils.h"
#include "GroupItem.h"
#include "CtrlCutScene.h"
#include "SimulatorDialog.h"
#include "LaserDialog.h"
#include "helpers/Qt.h"

class Document;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
  Q_OBJECT
public:
  ~MainWindow();

  static MainWindow *instance() { 
    if (!MainWindow::inst) MainWindow::inst = new MainWindow(); return MainWindow::inst; 
  }


public slots:
  void on_helpAboutAction_triggered();
  void on_fileOpenAction_triggered();
  void on_fileImportAction_triggered();
  void on_filePrintAction_triggered();
  void on_toolsMoveToOriginAction_triggered();
  void on_lpdclient_done(bool error);
  void on_lpdclient_progress(int done, int total);
  void on_simulateAction_triggered();
  void on_itemMoved(QGraphicsItem *item, const QPointF &moveStartPosition);
  void on_deleteItem();
  void on_simplifyItem();

  void sceneSelectionChanged();

  void openFile(const QString &filename);
  void importFile(const QString &filename);
  void simulate();
private:
  static MainWindow *inst;
  MainWindow();
  void createUndoView();
  void createActions();

  LpdClient *lpdclient;
  CtrlCutScene *scene;

  LaserDialog *laserdialog;
  SimulatorDialog *simdialog;

  QAction *undoAction;
  QAction *redoAction;

  QUndoStack *undoStack;
  QUndoView *undoView;
};

#endif
