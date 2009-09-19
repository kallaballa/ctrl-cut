/*
 * pcl.h
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */

#ifndef PCL_H_
#define PCL_H_

/* Raster Orientation */
const char* R_ORIENTATION = "\e*r0F";

/* Raster power */
const char* R_POWER = "\e&y%dP";

/* Raster speed */
const char* R_SPEED = "\e&z%dS";

/* Raster height */
const char* R_HEIGHT = "\e*r%dT";

/* Raster width */
const char* R_WIDTH = "\e*r%dS";

/* Raster compression */
const char* R_COMPRESSION = "\e*b%dM";

/* Raster direction (1 = up) */
const char* R_DIRECTION_UP = "\e&y1O";

/* Start raster operation at current position */
const char* R_START_AT_POS = "\e*r1A";

/* The intensity (relative amount of the defined R_POWER) used for the current raster row */
const char* R_INTENSITY = "\e*b%dA";

/* The number of bytes in the raster row */
const char* R_ROW_BYTES = "\e*b%dW";

/* Postition cursor absolute on the X-axis */
const char* POS_X = "\e*p%dY";

/* Postition cursor absolute on the Y-axis */
const char* POS_Y = "\e*p%dX";

/* Intialize vector mode */
const char* V_INIT = "IN;";

/* Set laser pulse frequency */
const char* V_FREQUENCY = "XR%04d;";

/* Set laser power */
const char* V_POWER = "YP%03d;";

/* Set laser speed */
const char* V_SPEED = "ZS%03d;";

/**
 * Pen up. This command doesnt comform to PCL/HPGL and is only used on the first issue of PU
 * There seems to be no difference to using the standard command also for the first issue, so
 * i use it only because the windows drivers does.
 * */
const char* V_PEN_UP_INIT = ";LTPU";

/* Pen up. */
const char* V_PEN_UP = ";PU";



#endif /* PCL_H_ */
