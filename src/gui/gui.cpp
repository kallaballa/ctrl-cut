#include <QApplication>
#include "MainWindow.h"
#include "EventFilter.h"
#include "util/Logger.h"
#include "config/DocumentSettings.h"

int main(int argc, char **argv)
{
  Logger::init(CC_DEBUG);
  QApplication app(argc, argv);
  app.installEventFilter(new EventFilter(&app));

  MainWindow::instance()->setGeometry(100, 100, 800, 500);
  MainWindow::instance()->show();

  return app.exec();
}
