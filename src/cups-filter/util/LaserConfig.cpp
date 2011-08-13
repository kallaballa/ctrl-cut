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
#include "LaserConfig.h"
#include "Logger.h"
#include <cups/cups.h>

/** Default power level for raster engraving */
#define RASTER_POWER_DEFAULT (20)

/** Default speed for raster engraving */
#define RASTER_SPEED_DEFAULT (100)

/** Default resolution in DPI */
#define RESOLUTION_DEFAULT (600)

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

LaserConfig::LaserConfig()
{
  this->tempdir = TMP_DIRECTORY;
  this->focus = DEFAULT_AUTO_FOCUS;
  this->height = BED_HEIGHT;
  this->width = BED_WIDTH;
  this->resolution = RESOLUTION_DEFAULT;
  this->device_width = calcDeviceDimension(this->width, this->resolution);
  this->device_height = calcDeviceDimension(this->height, this->resolution);
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
  calculate_base_position();
}

uint32_t LaserConfig::calcDeviceDimension(uint32_t dim, double res) {
  return round(((double)(dim * res)  / POINTS_PER_INCH));
}

void LaserConfig::setCupsOptions(cups_option_s *options, int numOptions)
{
  const char *v;
  if ((v = cupsGetOption("AutoFocus", numOptions, options))) {
    this->focus = strcmp(v, "false") != 0;
  }
  if ((v = cupsGetOption("Resolution", numOptions, options))) {
    this->resolution = atoi(v);
  }
  if ((v = cupsGetOption("RasterSpeed", numOptions, options))) {
    this->raster_speed = atoi(v);
  }
  if ((v = cupsGetOption("RasterPower", numOptions, options))) {
    this->raster_power = atoi(v);
  }
  if ((v = cupsGetOption("RasterDithering", numOptions, options))) {
    if (!strcmp(v, "Default")) this->raster_dithering = DITHER_DEFAULT;
    else if (!strcmp(v, "Bayer")) this->raster_dithering = DITHER_BAYER;
    else if (!strcmp(v, "FloydSteinberg")) this->raster_dithering = DITHER_FLOYD_STEINBERG;
    else if (!strcmp(v, "Jarvis")) this->raster_dithering = DITHER_JARVIS;
    else if (!strcmp(v, "Burke")) this->raster_dithering = DITHER_BURKE;
    else if (!strcmp(v, "Stucki")) this->raster_dithering = DITHER_STUCKI;
    else if (!strcmp(v, "Sierra2")) this->raster_dithering = DITHER_SIERRA2;
    else if (!strcmp(v, "Sierra3")) this->raster_dithering = DITHER_SIERRA3;
    else {
      LOG_WARN_MSG("Illegal value for RasterDithering", v);
    }
  }
  if ((v = cupsGetOption("RasterDirection", numOptions, options))) {
    if (!strcmp(v, "TopDown")) this->raster_direction = DIRECTION_TOPDOWN;
    else if (!strcmp(v, "BottomUp")) this->raster_direction = DIRECTION_BOTTOMUP;
    else {
      LOG_WARN_MSG("Illegal value for RasterDirection", v);
    }
  }
  if ((v = cupsGetOption("VectorSpeed", numOptions, options))) {
    this->vector_speed = atoi(v);
  }
  if ((v = cupsGetOption("VectorPower", numOptions, options))) {
    this->vector_power = atoi(v);
  }
  if ((v = cupsGetOption("VectorFrequency", numOptions, options))) {
    this->vector_freq = atoi(v);
  }
  if ((v = cupsGetOption("VectorReduce", numOptions, options))) {
    this->vector_reduce = atof(v);
  }
  if ((v = cupsGetOption("VectorOptimize", numOptions, options))) {
    if (!strcmp(v, "Simple")) this->vector_optimize = OPTIMIZE_SIMPLE;
    else if (!strcmp(v, "Inner-Outer")) this->vector_optimize = OPTIMIZE_INNER_OUTER;
    else if (!strcmp(v, "Shortest-Path")) this->vector_optimize = OPTIMIZE_SHORTEST_PATH;
    else {
      LOG_WARN_MSG("Illegal value for VectorOptimize", v);
    }
  }
  if ((v = cupsGetOption("Debug", numOptions, options))) {
    cc_loglevel = strcmp(v, "false") ? CC_DEBUG : (LogLevel)DEBUG;
  }
  if ((v = cupsGetOption("EnableRaster", numOptions, options))) {
    this->enable_raster = strcmp(v, "false");
  }
  if ((v = cupsGetOption("EnableVector", numOptions, options))) {
    this->enable_vector = strcmp(v, "false");
  }
  dumpDebug();
}

void LaserConfig::dumpDebug()
{
  LOG_DEBUG(this->focus);
  LOG_DEBUG(this->resolution);
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
    this->basex = this->x_center - this->width / 2;
  }
  if (this->y_center) {
    this->basey = this->y_center - this->height / 2;
  }
  if (this->basex < 0) {
    this->basex = 0;
  }
  if (this->basey < 0) {
    this->basey = 0;
  }
  // rasterises
  this->basex = LaserConfig::calcDeviceDimension(this->basex, this->resolution);
  this->basey = LaserConfig::calcDeviceDimension(this->basey, this->resolution);
}

