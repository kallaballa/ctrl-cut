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

#include <QApplication>
#include "MainWindow.h"
#include "EventFilter.h"
#include "util/Logger.hpp"
#include "config/DocumentSettings.hpp"

int main(int argc, char **argv)
{
  Logger::init(CC_DEBUG);
  QApplication::setGraphicsSystem("raster");
  QApplication app(argc, argv);

  app.installEventFilter(new EventFilter(&app));

  MainWindow::instance()->setGeometry(100, 100, 800, 500);
  MainWindow::instance()->show();

  for(int i = 1; i < argc; i++) {
    MainWindow::instance()->openFile(argv[i]);
  }
  return app.exec();
}
