#include <QApplication>
#include "MainWindow.h"
#include "EventFilter.h"
#include "util/Logger.hpp"
#include "config/DocumentSettings.hpp"

int main(int argc, char **argv)
{
  Logger::init(CC_DEBUG);
  QApplication app(argc, argv);
  app.installEventFilter(new EventFilter(&app));

  MainWindow::instance()->setGeometry(100, 100, 800, 500);
  MainWindow::instance()->show();

  for(int i = 0; i < argc; i++) {
    MainWindow::instance()->openFile(argv[i]);
  }
  return app.exec();
}
