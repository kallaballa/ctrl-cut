#ifndef LASER_CONFIG_H_
#define LASER_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "util/Logger.h"

/** Temporary directory to store files. */
#define TMP_DIRECTORY "tmp"

/** Default bed width (x-axis) in pts. */
#define BED_WIDTH 2592

/** Default bed height (y-axis) in pts. */
#define BED_HEIGHT 1728

/** Default points per inch for incoming data */
#define POINTS_PER_INCH (72)

class LaserConfig
{
public:
  LaserConfig();
  ~LaserConfig() {}
  void setCupsOptions(struct cups_option_s *options, int numOptions);
  void rangeCheck();

  /** Where to store temporary files */
  std::string tempdir;
  /** Folder where input file lives, /tmp if stdin */
  std::string datadir;
  std::string basename;

  /** Variable to track auto-focus. */
  bool focus;

  // FIXME: What is width, height actually used for? The object bbox
  // is stored here and used to be incorrectly sent as PCL R_WIDTH,
  // R_HEIGHT (which should really get bed size in all cases)
  /** Height of the image (y-axis). By default this is the bed's height. */
  int height;

  /** Width of the image (x-axis). By default this is the bed's width. */
  int width;

  /** Variable to track the resolution of the print. */
  int resolution;

  enum RasterDithering {
    DITHER_DEFAULT,
    DITHER_DARKEN,
    DITHER_THRESHOLD,
    DITHER_BAYER,
    DITHER_FLOYD_STEINBERG
  };

  RasterDithering raster_dithering;

  enum RasterDirection {
    DIRECTION_TOPDOWN = 0,
    DIRECTION_BOTTOMUP = 1
  };

  RasterDirection raster_direction;

  /** Variable to track the raster speed. */
  int raster_speed;

  /** Variable to track the raster power. */
  int raster_power;

  /** FIXME -- pixel size of screen, 0= threshold */
  int screen;

  /** Variable to track the vector speed. */
  int vector_speed;

  /** Variable to track the vector power. */
  int vector_power;

  /** Variable to track the vector frequency. FIXME */
  int vector_freq;

  /** Epsilon value for vector reduce filter */
  float vector_reduce;

  /** Which optimization to perform (Default: Inner-Outer) */
  enum VectorOptimize {
    OPTIMIZE_SIMPLE,
    OPTIMIZE_INNER_OUTER,
    OPTIMIZE_FLAT
  };
  VectorOptimize vector_optimize;

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

  // Are we running performing raster passes
  bool enable_raster;

  // Are we running performing vector passes
  bool enable_vector;

private:
  void calculate_base_position();
};

#endif /* LASER_CONFIG_H_ */
