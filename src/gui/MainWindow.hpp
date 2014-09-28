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

// Workaround for https://bugreports.qt-project.org/browse/QTBUG-22829
#ifdef Q_MOC_RUN
#define BOOST_LEXICAL_CAST_INCLUDED
#define BOOST_TT_HAS_OPERATOR_HPP_INCLUDED
#endif

#include <QMainWindow>
#include "ui_CtrlCut.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
  Q_OBJECT
public:
  ~MainWindow();

  static MainWindow *instance() { 
    if (!MainWindow::inst) MainWindow::inst = new MainWindow(); return MainWindow::inst; 
  }


public slots:
  void on_helpAboutAction_triggered();
  void on_fileNewAction_triggered();
  void on_fileOpenAction_triggered();
  void on_fileSaveAction_triggered();
  void on_fileSaveAsAction_triggered();
  void on_fileImportAction_triggered();
  void on_filePrintAction_triggered();
  void on_windowShowPropertiesAction_triggered();
  void on_toolsMoveToOriginAction_triggered();
  void on_lpdclient_done(bool error);
  void on_lpdclient_progress(int done, int total);
  void on_itemMoved(QGraphicsItem *item, const QPointF &moveStartPosition);
  void on_editDeleteItemAction_triggered();
  void showContextMenu(const QPoint& pos);
  void on_lowerItem();
  void on_raiseItem();
  void on_raiseItemToTop();
  void on_lowerItemToBottom();
  void sceneSelectionChanged();

  void on_undoStack_cleanChanged(bool);

  void openFile(const QString &filename);
  void importFile(const QString &filename);
  void saveFile(const QString &filename);
private:
  void closeEvent(QCloseEvent *event);
  bool maybeSave();

  static MainWindow *inst;
  MainWindow();
  void createUndoView();
  void createActions();
  void createContextMenu();

  class LpdClient *lpdclient;
  class CtrlCutScene *scene;

  class LaserDialog *laserdialog;
  class SimulatorDialog *simdialog;

  QAction *undoAction;
  QAction *redoAction;

  class QUndoStack *undoStack;
  class QUndoView *undoView;

  QMenu* menu;
  QAction* lowerAct;
  QAction* raiseAct;
  QAction* bottomAct;
  QAction* topAct;
};

#endif
