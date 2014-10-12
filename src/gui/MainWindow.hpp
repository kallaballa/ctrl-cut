#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

// Workaround for https://bugreports.qt-project.org/browse/QTBUG-22829
#ifdef Q_MOC_RUN
#define BOOST_LEXICAL_CAST_INCLUDED
#define BOOST_TT_HAS_OPERATOR_HPP_INCLUDED
#endif

#include <QMainWindow>
#include <QProgressDialog>
#include "GuiConfig.hpp"
#include "ui_CtrlCut.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
  Q_OBJECT
public:
  GuiConfig guiConfig;

  ~MainWindow();

  static MainWindow *instance() { 
    if (!MainWindow::inst) MainWindow::inst = new MainWindow(); return MainWindow::inst; 
  }


public slots:
  void on_helpAboutAction_triggered();
  void on_previewAction_triggered();
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
  void on_editCutAction_triggered();
  void on_editCopyAction_triggered();
  void on_editCopySettingsAction_triggered();
  void on_editPasteAction_triggered();
  void on_editPasteSettingsAction_triggered();
  void on_editGroupAction_triggered();
  void on_editUngroupAction_triggered();
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
  void saveGuiConfig();

private:
  void closeEvent(QCloseEvent *event);
  bool maybeSave();

  static MainWindow *inst;
  MainWindow();
  void createUndoView();
  void createActions();
  void createContextMenu();
  void loadGuiConfig();

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
  QProgressDialog progressDialog;
};

#endif
