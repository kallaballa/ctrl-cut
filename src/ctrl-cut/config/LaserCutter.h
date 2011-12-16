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

#ifndef LASERCUTTER_H_
#define LASERCUTTER_H_

#include "util/Logger.h"
#include "util/Measurement.h"
#include "encoder/PclEncoder.h"
#include "encoder/HPGLEncoder.h"

/**
 *  This class models the basic hardware specific parameters of a laser cutter.
 *  At the moment its a static configuration supporting only two laser cutters.
 *
 *  TODO: load laser cutter specifications from file.
 */
class LaserCutter {
private:
  // singleton - only one laser cutter model per ctrl-cut instance.
  static LaserCutter* instance;

public:
  typedef PclEncoder EngraveEncoder;
  typedef HPGLEncoder CutEncoder;

  enum Driver {
    UNINITIALIZED,
    EPILOG_LEGEND,
    EPILOG_ZING
  };

  Driver driver;
  Unit nativeUnit;
  bool supportsAutoFocus;

  LaserCutter(Driver driver) : driver(driver) {
    this->nativeUnit = IN;
    if(this->driver == EPILOG_LEGEND)
      this->supportsAutoFocus = true;
    else
      this->supportsAutoFocus = false;
  }


  // init must be called exactly once per ctrl-cut livetime and it must be called before the first invocation of getInstance.
  const LaserCutter& init(const Driver driver) {
    assert(this->instance == NULL);
    this->instance = new LaserCutter(driver);
    return *this->instance;
  }

  // return the global laser cutter model
  const LaserCutter& getInstance() {
    assert(this->instance != NULL);
    return *this->instance;
  }
};

#endif /* LASERCUTTER_H_ */
