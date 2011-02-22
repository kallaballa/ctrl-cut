#ifndef PJL_H_
#define PJL_H_

#ifdef PJL_PARAM
#  define PJL_STRING(_name, _text) extern const char *_name; const char *_name = _text;
#else
#  define PJL_STRING(_name, _text) extern const char *_name;
#endif

PJL_STRING(SEP, ";");

/**
 * PJL COMMANDS
 */

/* Print the printer job language header. */
PJL_STRING(PJL_HEADER, "\e%%-12345X@PJL JOB NAME=%s\r\n\eE@PJL ENTER LANGUAGE=PCL \r\n");

/* End job -> go back to PJL */
PJL_STRING(PJL_FOOTER, "\e%-12345X@PJL EOJ \r\n")


/**
 * PCL COMMANDS
 * **/

/* Set autofocus on or off. */
PJL_STRING(PCL_AUTOFOCUS, "\e&y%dA");

/* Left (long-edge) offset registration.  Adjusts the position of the
 * logical page across the width of the page.
 */
PJL_STRING(PCL_OFF_X, "\e&l%dU");

/* Top (short-edge) offset registration.  Adjusts the position of the
 * logical page across the length of the page.
 */
PJL_STRING(PCL_OFF_Y, "\e&l%dZ");

/* Resolution of the print. */
PJL_STRING(PCL_PRINT_RESOLUTION, "\e&u%dD");

/* PCL resolution. */
PJL_STRING(PCL_RESOLUTION, "\e*t%dR");

/* FIXME: unknown purpose. */
PJL_STRING(PCL_UNKNOWN_BLAFOO, "\e&y0C");

/* FIXME: unknown purpose. */
PJL_STRING(PCL_UNKNOWN_BLAFOO2, "\e&y0Z");

/* FIXME: unknown purpose. */
PJL_STRING(PCL_UNKNOWN_BLAFOO3, "\e&z2A");

/* Position cursor absolute on the X-axis */
PJL_STRING(PCL_POS_X, "\e*p%dX");

/* Position cursor absolute on the Y-axis */
PJL_STRING(PCL_POS_Y, "\e*p%dY");

/* PCL section end */
PJL_STRING(PCL_SECTION_END, "\e%1B");

/* Reset PCL */
PJL_STRING(PCL_RESET, "\eE");

/**
 * PCL RASTER COMMANDS
 * **/

/* Raster Orientation */
PJL_STRING(R_ORIENTATION, "\e*r%dF");

/* Raster power */
PJL_STRING(R_POWER, "\e&y%dP");

/* Raster speed */
PJL_STRING(R_SPEED, "\e&z%dS");

/* Raster height */
PJL_STRING(R_HEIGHT, "\e*r%dT");

/* Raster width */
PJL_STRING(R_WIDTH, "\e*r%dS");

/* Raster compression */
PJL_STRING(R_COMPRESSION, "\e*b%dM");

/* Raster direction (1 = up) */
PJL_STRING(R_DIRECTION_UP, "\e&y0O");

/* Start raster operation at current position */
PJL_STRING(R_START_AT_POS, "\e*r1A");

/* The intensity (relative amount of the defined R_POWER) used for the current raster row */
PJL_STRING(R_ROW_PIXELS, "\e*b%dA");

/* The number of bytes in the raster row */
PJL_STRING(R_ROW_BYTES, "\e*b%dW");

/* Guessed: resets raster job */
PJL_STRING(R_RESET, "\e*rC");


/**
 * PCL VECTOR COMMANDS
 * **/

/* Initialize vector mode */
PJL_STRING(V_INIT, "IN");

/* Set laser pulse frequency */
PJL_STRING(V_FREQUENCY, "XR%04d");

/* Set laser power */
PJL_STRING(V_POWER, "YP%03d");

/* Set laser speed */
PJL_STRING(V_SPEED, "ZS%03d");


/**
 * HPGL COMMANDS
 * **/

/**
 * Line Type
 * NB! The Windows driver inserts a Line Type without a parameter or
 * command terminator before the first Pen Up command. This doesn't
 * conform to PCL/HPGL and is only used on the first issue of PU in a
 * job. There seems to be no difference to using the standard command
 * also for the first issue, so I use it only because the windows
 * driver does.
 */
PJL_STRING(HPGL_LINE_TYPE, "LT");

/* Pen up */
PJL_STRING(HPGL_PEN_UP, "PU");

/* Pen down */
PJL_STRING(HPGL_PEN_DOWN, "PD");

/* HPGL section end */
PJL_STRING(HPGL_END, "\e%0B");

#endif /* PCL_H_ */
