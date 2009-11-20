/*
 * Laser.h
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */

#ifndef LASER_CONFIG_H_
#define LASER_CONFIG_H_


/** Default on whether or not auto-focus is enabled. */
#define AUTO_FOCUS (1)

/** Default bed height (y-axis) in pts. */
#define BED_HEIGHT (1728)

/** Default bed width (x-axis) in pts. */
#define BED_WIDTH (2592)

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

/** Default resolution is 600 DPI */
#define RESOLUTION_DEFAULT (600)

/** Pixel size of screen (0 is threshold).
 * FIXME - add more details
 */
#define SCREEN_DEFAULT (8)

/** FIXME */
#define VECTOR_FREQUENCY_DEFAULT (5000)

/** Default power level for vector cutting. */
#define VECTOR_POWER_DEFAULT (10)

/** Default speed level for vector cutting. */
#define VECTOR_SPEED_DEFAULT (100)

#endif /* LASER_CONFIG_H_ */
