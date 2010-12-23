#include "LaserConfig.h"

static void calculate_base_position(LaserConfig *lconf)
{
  if (lconf->x_center) {
    lconf->basex = lconf->x_center - lconf->width / 2;
  }
  if (lconf->y_center) {
    lconf->basey = lconf->y_center - lconf->height / 2;
  }
  if (lconf->basex < 0) {
    lconf->basex = 0;
  }
  if (lconf->basey < 0) {
    lconf->basey = 0;
  }
  // rasterises
  lconf->basex = lconf->basex * lconf->resolution / POINTS_PER_INCH;
  lconf->basey = lconf->basey * lconf->resolution / POINTS_PER_INCH;
}

void init_laser_config(LaserConfig *lconf)
{
  lconf->focus = AUTO_FOCUS;
  lconf->height = BED_HEIGHT;
  lconf->resolution = RESOLUTION_DEFAULT;
  lconf->raster_mode = RASTER_MODE_DEFAULT;
  lconf->raster_speed = RASTER_SPEED_DEFAULT;
  lconf->raster_power = RASTER_POWER_DEFAULT;
  lconf->raster_repeat = RASTER_REPEAT;
  lconf->screen = SCREEN_DEFAULT;
  lconf->vector_speed = VECTOR_SPEED_DEFAULT;
  lconf->vector_power = VECTOR_POWER_DEFAULT;
  lconf->vector_freq = VECTOR_FREQUENCY_DEFAULT;
  lconf->width = BED_WIDTH;
  lconf->x_repeat = 1;
  lconf->y_repeat = 1;
  lconf->basex = 0;
  lconf->basey = 0;
  lconf->flip = FLIP;
  calculate_base_position(lconf);
}
