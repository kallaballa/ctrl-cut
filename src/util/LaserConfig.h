#ifndef LASER_CONFIG_H_
#define LASER_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include "util/Logger.h"

/** Default on whether or not auto-focus is enabled. */
#define AUTO_FOCUS (1)

/** Default bed width (x-axis) in pts. */
#define BED_WIDTH 2592

/** Default bed height (y-axis) in pts. */
#define BED_HEIGHT 1728

/**
 * Default on whether or not the result is supposed to be flipped along the X
 * axis.
 */
#define FLIP (0)

/** Additional offset for the X axis. */
#define HPGLX (0)

/** Additional offset for the Y axis. */
#define HPGLY (0)

/** Accepted number of points per an inch. */
#define POINTS_PER_INCH (72)

/** Default mode for processing raster engraving (varying power depending upon
 * image characteristics).
 * Possible values are:
 * 'c' = color determines power level
 * 'g' = grey-scale levels determine power level
 * 'm' = mono mode
 * 'n' = no rasterization
 */
#define RASTER_MODE_DEFAULT 'g'

/** Default power level for raster engraving */
#define RASTER_POWER_DEFAULT (20)

/** Whether or not the raster printing is to be repeated. */
#define RASTER_REPEAT (1)

/** Default speed for raster engraving */
#define RASTER_SPEED_DEFAULT (100)

/** Default resolution in DPI */
#define RESOLUTION_DEFAULT (600)

/** Pixel size of screen (0 is threshold).
 * FIXME - add more details
 */
#define SCREEN_DEFAULT (8)

/** FIXME */
#define VECTOR_FREQUENCY_DEFAULT (1053)

/** Default power level for vector cutting. */
#define VECTOR_POWER_DEFAULT (80)

/** Default speed level for vector cutting. */
#define VECTOR_SPEED_DEFAULT (33)

struct LaserConfig
{
  /** Variable to track auto-focus. */
  int focus;

  // FIXME: What is width, height actually used for? The object bbox
  // is stored here and used to be incorrectly sent as PCL R_WIDTH,
  // R_HEIGHT (which should really get bed size in all cases)
  /** Height of the image (y-axis). By default this is the bed's height. */
  int height;

  /** Width of the image (x-axis). By default this is the bed's width. */
  int width;

  /** Variable to track the resolution of the print. */
  int resolution;

  /** Variable to track the mode for rasterization. One of color 'c', or
   * grey-scale 'g', mono 'm', or none 'n'
   */
  char raster_mode;

  /** Variable to track the raster speed. */
  int raster_speed;

  /** Variable to track the raster power. */
  int raster_power;

  /** Variable to track whether or not a rasterization should be repeated. */
  int raster_repeat;

  /** FIXME -- pixel size of screen, 0= threshold */
  int screen;

  /** Variable to track the vector speed. */
  int vector_speed;

  /** Variable to track the vector power. */
  int vector_power;

  /** Variable to track the vector frequency. FIXME */
  int vector_freq;

  /** X re-center (0 = not). */
  int x_center;

  /** Track whether or not to repeat X. */
  int x_repeat;

  /** Y re-center (0 = not). */
  int y_center;

  /** Track whether or not to repeat X. */
  int y_repeat;

  /** The relative x position inside the laser bed to start operating from **/
  int basex;

  /** The relative y position inside the laser bed to start operating from **/
  int basey;

  /** Variable to track whether or not the X axis should be flipped. */
  bool flip;

  // Are we running performing raster passes
  bool enable_raster;

  // Are we running performing vector passes
  bool enable_vector;
};

void init_laser_config(LaserConfig *lconf);

#endif /* LASER_CONFIG_H_ */
