#include <QApplication>
#include "MainWindow.h"
#include "util/Logger.h"
#include "util/LaserConfig.h"

LogLevel cc_loglevel = CC_WARNING;
/** Temporary buffer for building our strings. */
char buf[102400];

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  LaserConfig &conf = LaserConfig::inst();
  conf.enable_raster = true;
  conf.raster_dithering = LaserConfig::DITHER_FLOYD_STEINBERG;

  MainWindow mainwin;
  mainwin.setGeometry(100, 100, 800, 500);
  mainwin.show();

  return app.exec();
}
