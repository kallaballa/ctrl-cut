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
#include <iostream>
#include <sstream>
#include <string>
#include "raster/Raster.h"
#include "util/LaserConfig.h"
#include "LaserJob.h"
#include "Driver.h"

using boost::format;
using std::istringstream;
using std::string;

LogLevel cc_loglevel = (LogLevel)DEBUG;

/** Temporary buffer for building our strings. */
char buf[102400];

int stringtoint(string s) {
  istringstream iss(s);
  int i;
  iss >> i;
  return i;
}

void printUsage() {
  cerr << "Usage s:p:r:w:h:o:" << std::endl;
}

int main(int argc, char **argv) {
  int resolution = 600;
  int width = 2592;
  int height = 1728;
  int speed = 100;
  int power = 10;

  string* ifilename = NULL;
  string* ofilename = NULL;

  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "s:p:r:w:h:o:")) != -1)
    switch (c) {
    case 's':
      speed = stringtoint(optarg);
      break;
    case 'p':
      power = stringtoint(optarg);
      break;
    case 'r':
      resolution = stringtoint(optarg);
      break;
    case 'w':
      width = stringtoint(optarg);
      break;
    case 'h':
      height = stringtoint(optarg);
      break;
    case 'o':
      ofilename = new string(optarg);
      break;
    case '?':
      if (optopt == 'c')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      abort();
    }

  if (optind < argc) {
    ifilename = new string(argv[optind]);
  } else {
    printUsage();
    exit(1);
  }

  LaserConfig lconf;
  init_laser_config(&lconf);
  lconf.width = width;
  lconf.height = height;
  lconf.resolution = resolution;
  lconf.raster_speed = speed;
  lconf.raster_power = power;
  lconf.enable_raster = true;
  lconf.enable_vector = true;

  LOG_DEBUG(lconf.focus);
  LOG_DEBUG(lconf.resolution);
  LOG_DEBUG(lconf.raster_speed);
  LOG_DEBUG(lconf.raster_power);
  LOG_DEBUG(lconf.raster_mode);
  LOG_DEBUG(lconf.raster_repeat);
  LOG_DEBUG(lconf.vector_speed);
  LOG_DEBUG(lconf.vector_power);
  LOG_DEBUG(lconf.vector_freq);
  LOG_DEBUG(lconf.vector_optimize);
  LOG_DEBUG(lconf.flip);
  LOG_DEBUG(lconf.enable_raster);
  LOG_DEBUG(lconf.enable_vector);
  LaserJob job(&lconf, "ppm2pcl", "convert", *ifilename);
  Raster *raster = Raster::load(*ifilename);
  raster->addTile(raster->sourceImage);
  job.addRaster(raster);

  Driver drv;
  drv.process(&job);

  return 0;
}

