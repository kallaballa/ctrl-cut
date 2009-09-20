/*
 * pcl.h
 *
 *  Created on: 19.09.2009
 *      Author: amir
 */

#ifndef PJL_H_
#define PJL_H_

const char* SEP = ";";

/**
 * PJL COMMANDS
 * **/

/* Print the printer job language header. */
const char* PJL_HEADER =
		"\e%%-12345X@PJL JOB NAME=%s\r\n\eE@PJL ENTER LANGUAGE=PCL \r\n";

/* End job. */
const char* PJL_FOOTER = "@PJL EOJ \r\n";


/**
 * PCL COMMANDS
 * **/

/* Set autofocus on or off. */
const char* PCL_AUTOFOCUS = "\e&y%dA";

/* Left (long-edge) offset registration.  Adjusts the position of the
 * logical page across the width of the page.
 */
const char* PCL_OFF_X = "\e&l%dU";

/* Top (short-edge) offset registration.  Adjusts the position of the
 * logical page across the length of the page.
 */
const char* PCL_OFF_Y = "\e&l%dZ";

/* Resolution of the print. */
const char* PCL_PRINT_RESOLUTION = "\e&u%dD";

/* PCL resolution. */
const char* PCL_RESOLUTION = "\e*t%dR";

/* FIXME: unknown purpose. */
const char* PCL_UNKNOWN_BLAFOO =  "\e&y0C";

/* FIXME: unknown purpose. */
const char* PCL_UNKNOWN_BLAFOO2 =  "\e&y0Z";

/* FIXME: unknown purpose. */
const char* PCL_UNKNOWN_BLAFOO3 =  "\e&z2A";

/* Postition cursor absolute on the X-axis */
const char* PCL_POS_X = "\e*p%dY";

/* Postition cursor absolute on the Y-axis */
const char* PCL_POS_Y = "\e*p%dX";

/* PCL section end */
const char* PCL_SECTION_END = "\e%%1B";

/* Reset PCL */
const char* PCL_RESET = "\eE";

/* Exit PCL */
const char* PCL_EXIT = "\e%%-12345X";


/**
 * PCL RASTER COMMANDS
 * **/

/* Raster Orientation */
const char* R_ORIENTATION = "\e*r%dF";

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

/* Guessed: resets raster job */
const char* R_RESET = "\e*rC";


/**
 * PCL VECTOR COMMANDS
 * **/

/* Intialize vector mode */
const char* V_INIT = "IN";

/* Set laser pulse frequency */
const char* V_FREQUENCY = "XR%04d";

/* Set laser power */
const char* V_POWER = "YP%03d";

/* Set laser speed */
const char* V_SPEED = "ZS%03d";


/**
 * HPGL COMMANDS
 * **/

/**
 * Pen up. This command doesnt comform to PCL/HPGL and is only used on the first issue of PU
 * There seems to be no difference to using the standard command also for the first issue, so
 * i use it only because the windows driver does.
 * */
const char* HPGL_PEN_UP_INIT = "LTPU";

/* Pen up. */
const char* HPGL_PEN_UP = "PU";

/* Pen up. */
const char* HPGL_PEN_DOWN = "PD";

/* HPGL section end */
const char* HPGL_END = "\e%%0B";

#endif /* PCL_H_ */
