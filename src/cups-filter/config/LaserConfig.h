#ifndef LASER_CONFIG_H_
#define LASER_CONFIG_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <cups/cups.h>
#include "util/Logger.h"
#include "util/Measurement.h"
#include "LaserCutter.h"

/** Temporary directory to store files. */
#define TMP_DIRECTORY "tmp"

class LaserConfig
{
private:
  LaserConfig();
  void loadDefaults();
  void calculate_base_position();

  static LaserConfig *instance;
public:
  enum RasterDithering {
    DITHER_DEFAULT,
    DITHER_BAYER,
    DITHER_FLOYD_STEINBERG,
    DITHER_JARVIS,
    DITHER_BURKE,
    DITHER_STUCKI,
    DITHER_SIERRA2,
    DITHER_SIERRA3
  };
  enum RasterDirection {
    DIRECTION_TOPDOWN = 0,
    DIRECTION_BOTTOMUP = 1
  };

  enum VectorOptimize {
    OPTIMIZE_SIMPLE,
    OPTIMIZE_INNER_OUTER,
    OPTIMIZE_SHORTEST_PATH
  };

  // the configured laser cutter hardware
  LaserCutter laserCutter;
  RasterDithering raster_dithering;
  RasterDirection raster_direction;

  /** Which optimization to perform (Default: Inner-Outer) */
  VectorOptimize vector_optimize;

  /** Where to store temporary files */
  std::string tempdir;
  /** Folder where input file lives, /tmp if stdin */
  std::string datadir;
  std::string basename;

  /** Variable to track auto-focus. */
  bool focus;

  /** Variable to track the resolution of the print. */
  int resolution;

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

  ~LaserConfig() {}

  static LaserConfig &getInstance() {
    assert(instance != NULL);
    return *instance;
  }

  static void initFromCups(cups_option_s *options, int numOptions);
  const uint32_t getGraphicsDeviceWidth() const;
  const uint32_t getGraphicsDeviceHeight() const;
  void rangeCheck();
  void dumpDebug();
};

#endif /* LASER_CONFIG_H_ */
