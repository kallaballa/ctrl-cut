/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include <QtDebug>
#include <QApplication>
#include <QUndoView>
#include "MainWindow.hpp"
#include "EventFilter.hpp"
#include "util/Logger.hpp"
#include "config/DocumentSettings.hpp"

class CtrlCutApplication : public QApplication {
public:
  CtrlCutApplication(int& argc, char ** argv) :
    QApplication(argc, argv) { }
  virtual ~CtrlCutApplication() { }

  // reimplemented from QApplication so we can throw exceptions in slots
  virtual bool notify(QObject * receiver, QEvent * event) {
    try {
      return QApplication::notify(receiver, event);
    } catch(std::exception& e) {
      qCritical() << "Exception thrown:" << e.what();
    }
    return false;
  }
};

int main(int argc, char **argv)
{
  Logger::init(CC_DEBUG);

  //  QApplication::setGraphicsSystem("raster");
  CtrlCutApplication app(argc, argv);

  app.installEventFilter(new EventFilter(&app));

  QObject::connect(&app, SIGNAL(aboutToQuit()), MainWindow::instance(), SLOT(saveGuiConfig()));
  MainWindow::instance()->setGeometry(100, 100, 800, 500);
  MainWindow::instance()->show();
  // FIXME: Unified toolbar doesn't work with the "raster" graphics system on Qt-4.8.6
  MainWindow::instance()->setUnifiedTitleAndToolBarOnMac(false);

  for(int i = 1; i < argc; i++) {
    MainWindow::instance()->openFile(argv[i]);
  }
  return app.exec();
}
