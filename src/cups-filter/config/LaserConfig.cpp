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
#include <math.h>
#include <cups/cups.h>
#include <boost/lexical_cast.hpp>
#include "LaserConfig.h"
#include "util/Logger.h"
#include "util/Measurement.h"
#include "config/CupsOptions.h"

using boost::lexical_cast;

/** Default power level for raster engraving */
#define RASTER_POWER_DEFAULT (20)

/** Default speed for raster engraving */
#define RASTER_SPEED_DEFAULT (100)

/** Pixel size of screen (0 is threshold).
 * FIXME - add more details
 */
#define SCREEN_DEFAULT (8)

#define VECTOR_FREQUENCY_DEFAULT (5000)

/** Default power level for vector cutting. */
#define VECTOR_POWER_DEFAULT (80)

/** Default speed level for vector cutting. */
#define VECTOR_SPEED_DEFAULT (33)

/** Default on whether or not auto-focus is enabled. */
#define DEFAULT_AUTO_FOCUS (1)

LaserConfig *LaserConfig::instance = NULL;

LaserConfig::LaserConfig() : laserCutter(LaserCutter::UNINITIALIZED) {
  loadDefaults();
  calculate_base_position();
}

void LaserConfig::loadDefaults() {
  this->tempdir = TMP_DIRECTORY;
  this->focus = DEFAULT_AUTO_FOCUS;
  this->resolution = -1;
  this->raster_dithering = LaserConfig::DITHER_DEFAULT;
  this->raster_direction = LaserConfig::DIRECTION_TOPDOWN;
  this->raster_speed = RASTER_SPEED_DEFAULT;
  this->raster_power = RASTER_POWER_DEFAULT;
  this->screen = SCREEN_DEFAULT;
  this->vector_speed = VECTOR_SPEED_DEFAULT;
  this->vector_power = VECTOR_POWER_DEFAULT;
  this->vector_freq = VECTOR_FREQUENCY_DEFAULT;
  this->vector_reduce = 0.0f;
  this->vector_optimize = OPTIMIZE_INNER_OUTER;
  this->x_center = 0;
  this->x_repeat = 1;
  this->y_center = 0;
  this->y_repeat = 1;
  this->basex = 0;
  this->basey = 0;
  this->enable_vector = true;
  this->enable_raster = false;
}
/*!
 Copy supported options into the supplied laser_configa
 */
LaserConfig& LaserConfig::parseCupsOptions(cups_option_s *options, int numOptions)
{
  CupsOptions cupsOptions(options, numOptions);
  LaserConfig& config = *(new LaserConfig());
  string v;

  if (cupsOptions.get(DRIVER,v)) {
    if (v == "EpilogLegend") {
      config.laserCutter = *(new LaserCutter(LaserCutter::EPILOG_LEGEND));
    }
    else if (v == "EpilogZing") {
      config.laserCutter = *(new LaserCutter(LaserCutter::EPILOG_ZING));
    }
    else {
      LOG_WARN_MSG("Illegal value for Driver", v);
    }
  }
  if (cupsOptions.get(BEDSIZE,v)) {
    if (v == "16x12") {
      config.laserCutter.setBedSize(16,12, IN);
    }
    else if (v == "24x12") {
      config.laserCutter.setBedSize(24,12, IN);
    }
    else if (v == "36x24") {
      config.laserCutter.setBedSize(36,24, IN);
    }
    else {
      LOG_WARN_MSG("Illegal value for BedSize", v);
    }
  }

  if (cupsOptions.get(AUTOFOCUS,v)) {
    config.focus = v != "false";
  }
  if (cupsOptions.get(RESOLUTION,v)) {
    config.resolution = lexical_cast<uint16_t>(v);
  }
  if (cupsOptions.get(RASTER_SPEED,v)) {
    config.raster_speed = lexical_cast<uint16_t>(v);
  }
  if (cupsOptions.get(RASTER_POWER,v)) {
    config.raster_power = lexical_cast<uint16_t>(v);
  }
  if (cupsOptions.get(RASTER_DITHERING,v)) {
    if (v == "Default") config.raster_dithering = DITHER_DEFAULT;
    else if (v == "Bayer") config.raster_dithering = DITHER_BAYER;
    else if (v == "FloydSteinberg") config.raster_dithering = DITHER_FLOYD_STEINBERG;
    else if (v == "Jarvis") config.raster_dithering = DITHER_JARVIS;
    else if (v == "Burke") config.raster_dithering = DITHER_BURKE;
    else if (v == "Stucki") config.raster_dithering = DITHER_STUCKI;
    else if (v == "Sierra2") config.raster_dithering = DITHER_SIERRA2;
    else if (v == "Sierra3") config.raster_dithering = DITHER_SIERRA3;
    else {
      LOG_WARN_MSG("Illegal value for RasterDithering", v);
    }
  }
  if (cupsOptions.get(RASTER_DIRECTION,v)) {
    if (v == "TopDown") config.raster_direction = DIRECTION_TOPDOWN;
    else if (v == "BottomUp") config.raster_direction = DIRECTION_BOTTOMUP;
    else {
      LOG_WARN_MSG("Illegal value for RasterDirection", v);
    }
  }
  if (cupsOptions.get(VECTOR_SPEED,v)) {
    config.vector_speed = lexical_cast<uint16_t>(v);
  }
  if (cupsOptions.get(VECTOR_POWER,v)) {
    config.vector_power = lexical_cast<uint16_t>(v);
  }
  if (cupsOptions.get(VECTOR_FREQUENCY,v)) {
    config.vector_freq = lexical_cast<uint16_t>(v);
  }
  if (cupsOptions.get(VECTOR_REDUCE,v)) {
    config.vector_reduce = lexical_cast<uint16_t>(v);
  }
  if (cupsOptions.get(VECTOR_OPTIMIZE,v)) {
    if (v == "Simple") config.vector_optimize = OPTIMIZE_SIMPLE;
    else if (v == "Inner-Outer") config.vector_optimize = OPTIMIZE_INNER_OUTER;
    else if (v == "Shortest-Path") config.vector_optimize = OPTIMIZE_SHORTEST_PATH;
    else {
      LOG_WARN_MSG("Illegal value for VectorOptimize", v);
    }
  }
  if (cupsOptions.get(ENABLE_DEBUG,v)) {
    cc_loglevel = v != "false" ? CC_DEBUG : (LogLevel)DEBUG;
  }
  if (cupsOptions.get(ENABLE_RASTER,v)) {
    config.enable_raster = v != "false";
  }
  if (cupsOptions.get(ENABLE_VECTOR,v)) {
    config.enable_vector = v != "false";
  }

  if (config.laserCutter.driver == LaserCutter::UNINITIALIZED) {
    LOG_FATAL_STR("Driver not specified.");
  }

  if (config.resolution <= 0) {
    LOG_FATAL_STR("Resolution not specified.");
  }

  LaserConfig::instance = &config;
  config.dumpDebug();
  return config;
}

void LaserConfig::dumpDebug()
{
  LOG_DEBUG(this->laserCutter.driver);
  LOG_DEBUG(this->resolution);
  LOG_DEBUG(this->focus);
  LOG_DEBUG(this->raster_speed);
  LOG_DEBUG(this->raster_power);
  LOG_DEBUG(this->raster_dithering);
  LOG_DEBUG(this->raster_direction);
  LOG_DEBUG(this->vector_speed);
  LOG_DEBUG(this->vector_power);
  LOG_DEBUG(this->vector_freq);
  LOG_DEBUG(this->vector_optimize);
  LOG_DEBUG(this->enable_raster);
  LOG_DEBUG(this->enable_vector);
}

/*!
 * Perform range validation checks on the major global variables to ensure
 * their values are sane. If values are outside accepted tolerances then modify
 * them to be the correct value.
 *
 * @return Nothing
 */
void LaserConfig::rangeCheck()
{
  if (this->raster_power > 100) {
    this->raster_power = 100;
  }
  if (this->raster_power < 0) {
    this->raster_power = 0;
  }

  if (this->raster_speed > 100) {
    this->raster_speed = 100;
  }
  if (this->raster_speed < 1) {
    this->raster_speed = 1;
  }

  if (this->resolution > 1200) {
    this->resolution = 1200;
  }
  if (this->resolution < 75) {
    this->resolution = 75;
  }

  if (this->screen < 1) {
    this->screen = 1;
  }

  if (this->vector_freq < 10) {
    this->vector_freq = 10;
  }
  if (this->vector_freq > 5000) {
    this->vector_freq = 5000;
  }

  if (this->vector_power > 100) {
    this->vector_power = 100;
  }
  if (this->vector_power < 0) {
    this->vector_power = 0;
  }

  if (this->vector_speed > 100) {
    this->vector_speed = 100;
  }
  if (this->vector_speed < 1) {
    this->vector_speed = 1;
  }
}

void LaserConfig::calculate_base_position()
{
  if (this->x_center) {
    this->basex = this->x_center - getGraphicsDeviceWidth() / 2;
  }
  if (this->y_center) {
    this->basey = this->y_center - getGraphicsDeviceHeight() / 2;
  }
  if (this->basex < 0) {
    this->basex = 0;
  }
  if (this->basey < 0) {
    this->basey = 0;
  }
  // rasterises
  this->basex = this->basex * this->resolution / 72;
  this->basey = this->basey * this->resolution / 72;
}

const double LaserConfig::getGraphicsDeviceWidth() const {
  return this->laserCutter.bedSize.width.in(PX, 72) * this->resolution;
}

const double LaserConfig::getGraphicsDeviceHeight() const {
  return this->laserCutter.bedSize.height.in(PX, 72) * this->resolution;
}
