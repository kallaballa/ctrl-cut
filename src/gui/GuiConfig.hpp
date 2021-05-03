/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef SRC_GUI_GUICONFIG_HPP_
#define SRC_GUI_GUICONFIG_HPP_

#include "config/LaserCutter.hpp"
#include "util/2D.hpp"

struct GuiConfig {
  LaserCutter::Driver driver = LaserCutter::EPILOG_LEGEND;
  Unit unit = MM;
  double resolution = 25400;
  double bedWidth = 1300;
  double bedHeight = 900;
  string networkAddress;
  string version;
};

#endif /* SRC_GUI_GUICONFIG_HPP_ */
