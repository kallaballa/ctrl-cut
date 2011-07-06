#include <QApplication>
#include "MainWindow.h"
#include "EventFilter.h"
#include "util/Logger.h"
#include "util/LaserConfig.h"

LogLevel cc_loglevel = CC_DEBUG;
/** Temporary buffer for building our strings. */
char buf[102400];

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  app.installEventFilter(new EventFilter(&app));

  LaserConfig &conf = LaserConfig::inst();
  conf.enable_raster = true;
  conf.raster_dithering = LaserConfig::DITHER_FLOYD_STEINBERG;

  MainWindow::instance()->setGeometry(100, 100, 800, 500);
  MainWindow::instance()->show();

  return app.exec();
}
