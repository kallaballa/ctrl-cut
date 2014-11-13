/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef PJL_H_
#define PJL_H_

#ifdef PJL_PARAM
#  define PJL_STRING(_name, _text) extern const char *_name; const char *_name = _text;
#else
#  define PJL_STRING(_name, _text) extern const char *_name;
#endif

PJL_STRING(SEP, ";")

/**
 * PJL COMMANDS
 */

/* Print the printer job language header. */
PJL_STRING(PJL_HEADER, "\x1B%%-12345X@PJL JOB NAME=%s\r\n\x1B\x45@PJL ENTER LANGUAGE=PCL \r\n")

/* End job -> go back to PJL */
PJL_STRING(PJL_FOOTER, "\x1B%-12345X@PJL EOJ \r\n")


/**
 * PCL COMMANDS
 * **/

/* Set autofocus on or off. */
PJL_STRING(PCL_AUTOFOCUS, "\x1B&y%dA")

/* Left (long-edge) offset registration.  Adjusts the position of the
 * logical page across the width of the page.
 */
PJL_STRING(PCL_OFF_X, "\x1B&l%dU")

/* Top (short-edge) offset registration.  Adjusts the position of the
 * logical page across the length of the page.
 */
PJL_STRING(PCL_OFF_Y, "\x1B&l%dZ")

// issued after a center engraving. don't understand what id exactly does
PJL_STRING(PCL_UPPERLEFT_X, "\x1B&l%dW")
PJL_STRING(PCL_UPPERLEFT_Y, "\x1B&l%dV")

/* Resolution of the print. */
PJL_STRING(PCL_PRINT_RESOLUTION, "\x1B&u%dD")

/* PCL resolution. */
PJL_STRING(PCL_RESOLUTION, "\x1B*t%dR")

/* enable center engraving */
PJL_STRING(PCL_CENTER_ENGRAVE, "\x1B&y%dZ")

/* FIXME: unknown purpose. */
PJL_STRING(PCL_UNKNOWN_BLAFOO, "\x1B&y0C")

/* FIXME: unknown purpose. */
PJL_STRING(PCL_UNKNOWN_BLAFOO3, "\x1B&z2A")

/* Position cursor absolute on the X-axis */
PJL_STRING(PCL_POS_X, "\x1B*p%dX")

/* Position cursor absolute on the Y-axis */
PJL_STRING(PCL_POS_Y, "\x1B*p%dY")

/* PCL section end */
PJL_STRING(PCL_SECTION_END, "\x1B%1B")

/* Reset PCL */
PJL_STRING(PCL_RESET, "\x1B\x45")

/**
 * PCL RASTER COMMANDS
 * **/

/* Raster Orientation */
PJL_STRING(R_ORIENTATION, "\x1B*r%dF")

/* Raster power */
PJL_STRING(R_POWER, "\x1B&y%dP")

/* Raster speed */
PJL_STRING(R_SPEED, "\x1B&z%dS")

/* Bed height */
PJL_STRING(R_BED_HEIGHT, "\x1B*r%dT")

/* Bed width */
PJL_STRING(R_BED_WIDTH, "\x1B*r%dS")

/* Raster compression */
PJL_STRING(R_COMPRESSION, "\x1B*b%dM")

/* Raster direction (0 = down, 1 = up) */
PJL_STRING(R_DIRECTION, "\x1B&y%dO")

/* Start raster job */
PJL_STRING(R_START, "\x1B*r1A")

/* End raster job */
PJL_STRING(R_END, "\x1B*rC")

/* The number of unpacked bytes in the raster row */
PJL_STRING(R_ROW_UNPACKED_BYTES, "\x1B*b%dA")

/* The number of packed bytes in the raster row */
PJL_STRING(R_ROW_PACKED_BYTES, "\x1B*b%dW")



/**
 * PCL VECTOR COMMANDS
 * **/

/* Initialize vector mode */
PJL_STRING(V_INIT, "IN")

/* Set laser pulse frequency */
PJL_STRING(V_FREQUENCY, "XR%04d")

/* Set laser power */
PJL_STRING(V_POWER, "YP%03d")

/* Set laser speed */
PJL_STRING(V_SPEED, "ZS%03d")


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
PJL_STRING(HPGL_LINE_TYPE, "LT")

/* Pen up */
PJL_STRING(HPGL_PEN_UP, "PU")

/* Pen down */
PJL_STRING(HPGL_PEN_DOWN, "PD")

/* HPGL section end */
PJL_STRING(HPGL_END, "\x1B%0B")

#endif /* PCL_H_ */
