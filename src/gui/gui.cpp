#include <QApplication>
#include "MainWindow.h"
#include "util/Logger.h"

LogLevel cc_loglevel = CC_WARNING;

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  MainWindow mainwin;
  mainwin.setGeometry(100, 100, 800, 500);
  mainwin.show();

  return app.exec();
}
