/* @file cups-epilog.c @verbatim
 *========================================================================
 * E&OE Copyright © 2002-2008 Andrews & Arnold Ltd <info@aaisp.net.uk>
 * Copyright 2008 AS220 Labs <brandon@as220.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <	>.
 *========================================================================
 *
 *
 * Author: Andrew & Arnold Ltd and Brandon Edens
 *
 *
 * Description:
 * Epilog laser engraver
 *
 * The Epilog laser engraver comes with a windows printer driver. This works
 * well with Corel Draw, and that is about it. There are other windows
 * applications, like inkscape, but these rasterise the image before sending to
 * the windows printer driver, so there is no way to use them to vector cut!
 *
 * The cups-epilog app is a cups backend, so build and link/copy to
 * /usr/lib/cups/backend/epilog. It allows you to print postscript to the laser
 * and both raster and cut. It works well with inkscape.
 *
 * With this linux driver, vector cutting is recognised by any line or curve in
 * 100% red (1.0 0.0 0.0 setrgbcolor).
 *
 * Create printers using epilog://host/Legend/options where host is the
 * hostname or IP of the epilog engraver. The options are as follows. This
 * allows you to make a printer for each different type of material.
 * af	Auto focus (0=no, 1=yes)
 * r	Resolution 75-1200
 * rs	Raster speed 1-100
 * rp	Raster power 0-100
 * vs	Vector speed 1-100
 * vp	Vector power 1-100
 * vf	Vector frequency 10-5000
 * sc	Photograph screen size in pizels, 0=threshold, +ve=line, -ve=spot, used
 *      in mono mode, default 8.
 * rm	Raster mode mono/grey/colour
 *
 * The mono raster mode uses a line or dot screen on any grey levels or
 * colours. This can be controlled with the sc parameter. The default is 8,
 * which makes a nice fine line screen on 600dpi engraving. At 600/1200 dpi,
 * the image is also lightened to allow for the size of the laser point.
 *
 * The grey raster mode maps the grey level to power level. The power level is
 * scaled to the raster power setting (unlike the windows driver which is
 * always 100% in 3D mode).
 *
 * In colour mode, the primary and secondary colours are processed as separate
 * passes, using the grey level of the colour as a power level. The power level
 * is scaled to the raster power setting. Note that red is 100% red, and non
 * 100% green and blue, etc, so 50% red, 0% green/blue is not counted as red,
 * but counts as "grey". 100% red, and 50% green/blue counts as red, half
 * power. This means you can make distinct raster areas of the page so that you
 * do not waste time moving the head over blank space between them.
 *
 * Epolog cups driver
 * Uses gs to rasterise the postscript input.
 * URI is epilog://host/Legend/options
 * E.g. epilog://host/Legend/rp=100/rs=100/vp=100/vs=10/vf=5000/rm=mono/flip/af
 * Options are as follows, use / to separate :-
 * rp   Raster power
 * rs   Raster speed
 * vp   Vector power
 * vs   Vector speed
 * vf   Vector frequency
 * w    Default image width (pt)
 * h    Default image height (pt)
 * sc   Screen (lpi = res/screen, 0=simple threshold)
 * r    Resolution (dpi)
 * af   Auto focus
 * rm   Raster mode (mono/grey/colour)
 * flip X flip (for reverse cut)
 * Raster modes:-
 * mono Screen applied to grey levels
 * grey Grey levels are power (scaled to raster power setting)
 * colour       Each colour grey/red/green/blue/cyan/magenta/yellow plotted
 * separately, lightness=power
 *
 *
 * Installation:
 * gcc -o epilog `cups-config --cflags` cups-epilog.c `cups-config --libs`
 * http://www.cups.org/documentation.php/api-overview.html
 *
 * Manual testing can be accomplished through execution akin to:
 * $ export DEVICE_URI="epilog://epilog-mini/Legend/rp=100/rs=100/vp=100/vs=10/vf=5000/rm=grey"
 * # ./epilog job user title copies options
 * $ ./epilog 123 jdoe test 1 options < hello-world.ps
 *
 */


/*************************************************************************
 * includes
 */
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "Socket.h"
#include "ClientSocket.h"
#include "laser_config.h"
#include "PrinterConnection.h"
#include "cups_epilog.h"
#include "CuttingOptimizer.h"
#include "pjl.h"

/** The printer job **/
printer_job pjob;

/** The laser cutter configuration **/
laser_config lconf;

/** Temporary buffer for building out strings. */
char buf[102400];

/** Determines whether or not debug is enabled. */
char debug = DEBUG;

/** The DEVICE_URI for the printer. */
char *device_uri = "";

char *queue_options = "";

FILE* pjl_file;

/*************************************************************************/

/**
 * Convert a big endian value stored in the array starting at the given pointer
 * position to its little endian value.
 *
 * @param position the starting location for the conversion. Each successive
 * unsigned byte is upto nbytes is considered part of the value.
 * @param nbytes the number of successive bytes to convert.
 *
 * @return An integer containing the little endian value of the successive
 * bytes.
 */
int
big_to_little_endian(uint8_t *position, int nbytes)
{
    int i;
    int result = 0;

    for (i = 0; i < nbytes; i++) {
        result += *(position + i) << (8 * i);
    }
    return result;
}

/**
 * Execute ghostscript feeding it an ecapsulated postscript file which is then
 * converted into a bitmap image. As a byproduct output of the ghostscript
 * process is redirected to a .vector file which will contain instructions on
 * how to perform a vector cut of lines within the postscript.
 *
 * @param filename_bitmap the filename to use for the resulting bitmap file.
 * @param filename_eps the filename to read in encapsulated postscript from.
 * @param filename_vector the filename that will contain the vector
 * information.
 * @param bmp_mode a string which is one of bmp16m, bmpgray, or bmpmono.
 * @param resolution the encapsulated postscript resolution.
 * @param height the postscript height in points per inch.
 * @param width the postscript width in points per inch.
 *
 * @return Return true if the execution of ghostscript succeeds, false
 * otherwise.
 */
bool
execute_ghostscript(char *filename_bitmap,
                    char *filename_eps,
                    char *filename_vector,
                    char *bmp_mode, int resolution,
                    int height, int width)
{
    char buf[8192];
    sprintf(buf,"/usr/bin/gs -q -dBATCH -dNOPAUSE -r%d -g%dx%d -sDEVICE=%s -sOutputFile=%s %s > %s",
            resolution,
            (width * resolution) / POINTS_PER_INCH,
            (height * resolution) / POINTS_PER_INCH,
            bmp_mode,
            filename_bitmap,
            filename_eps,
            filename_vector);

    if (debug) {
        fprintf(stderr, "%s\n", buf);
    }
    if (system(buf)) {
        return false;
    }
    return true;
}

/*bool
execute_gtklp(char *printer_name,char *filename_cups, char *gtkconfdir)
{
    char buf[8192];
    setenv("DISPLAY", ":20.0", 1);
    sprintf(buf,"/usr/bin/gtklp -c %s -P %s < %s", gtkconfdir, printer_name, filename_cups);

    if (debug) {
        fprintf(stderr, "%s\n", buf);
    }
    if (system(buf)) {
        return false;
    }
    return true;
}*/

/**
 *
 */
bool
generate_raster(printer_job *pjob, laser_config *lconf ,FILE *bitmap_file)
{
    int h;
    int d;
    int offx;
    int offy;
    int repeat;
    uint8_t bitmap_header[BITMAP_HEADER_NBYTES];

    repeat = lconf->raster_repeat;
    while (repeat--) {
        /* repeated (over printed) */
        int pass;
        int passes;
        long base_offset;
        if (lconf->raster_mode == 'c') {
            passes = 7;
        } else {
            passes = 1;
        }

        /* Read in the bitmap header. */
        fread(bitmap_header, 1, BITMAP_HEADER_NBYTES, bitmap_file);

        /* Re-load width/height from bmp as it is possible that someone used
         * setpagedevice or some such
         */
        /* Bytes 18 - 21 are the bitmap width (little endian format). */
        lconf->width = big_to_little_endian(bitmap_header + 18, 4);

        /* Bytes 22 - 25 are the bitmap height (little endian format). */
        lconf->height = big_to_little_endian(bitmap_header + 22, 4);

        /* Bytes 10 - 13 base offset for the beginning of the bitmap data. */
        base_offset = big_to_little_endian(bitmap_header + 10, 4);


        if (lconf->raster_mode == 'c' || lconf->raster_mode == 'g') {
            /* colour/grey are byte per pixel power levels */
            h = lconf->width;
            /* BMP padded to 4 bytes per scan line */
            d = (h * 3 + 3) / 4 * 4;
        } else {
            /* mono */
            h = (lconf->width + 7) / 8;
            /* BMP padded to 4 bytes per scan line */
            d = (h + 3) / 4 * 4;
        }
        if (debug) {
            fprintf(stderr, "Width %d Height %d Bytes %d Line %d\n",
            		lconf->width, lconf->height, h, d);
        }

        /* Raster Orientation */
        fprintf(pjl_file, R_ORIENTATION, 0);
        /* Raster power */
        fprintf(pjl_file, R_POWER,
                (lconf->raster_mode == 'c' ||
                		lconf->raster_mode == 'g') ? 100 : lconf->raster_power);
        /* Raster speed */
        fprintf(pjl_file, PCL_UNKNOWN_BLAFOO3);
        fprintf(pjl_file, R_SPEED, lconf->raster_speed);
        fprintf(pjl_file, R_HEIGHT, lconf->height * lconf->y_repeat);
        fprintf(pjl_file, R_WIDTH, lconf->width * lconf->x_repeat);
        /* Raster compression */
        fprintf(pjl_file, R_COMPRESSION, (lconf->raster_mode == 'c' || lconf->raster_mode == 'g')
                ? 7 : 2);
        /* Raster direction (1 = up) */
        fprintf(pjl_file, R_DIRECTION_UP);
        /* start at current position */
        fprintf(pjl_file, R_START_AT_POS);
        for (offx = lconf->width * (lconf->x_repeat - 1); offx >= 0; offx -= lconf->width) {
            for (offy = lconf->height * (lconf->y_repeat - 1); offy >= 0; offy -= lconf->height) {
                for (pass = 0; pass < passes; pass++) {
                    // raster (basic)
                    int y;
                    char dir = 0;

                    fseek(bitmap_file, base_offset, SEEK_SET);
                    for (y = lconf->height - 1; y >= 0; y--) {
                        int l;

                        switch (lconf->raster_mode) {
                        case 'c':      // colour (passes)
                        {
                            char *f = buf;
                            char *t = buf;
                            if (d > sizeof (buf)) {
                                perror("Too wide");
                                return false;
                            }
                            l = fread ((char *)buf, 1, d, bitmap_file);
                            if (l != d) {
                                fprintf(stderr, "Bad bit data from gs %d/%d (y=%d)\n", l, d, y);
                                return false;
                            }
                            while (l--) {
                                // pack and pass check RGB
                                int n = 0;
                                int v = 0;
                                int p = 0;
                                int c = 0;
                                for (c = 0; c < 3; c++) {
                                    if (*f > 240) {
                                        p |= (1 << c);
                                    } else {
                                        n++;
                                        v += *f;
                                    }
                                    f++;
                                }
                                if (n) {
                                    v /= n;
                                } else {
                                    p = 0;
                                    v = 255;
                                }
                                if (p != pass) {
                                    v = 255;
                                }
                                *t++ = 255 - v;
                            }
                        }
                        break;
                        case 'g':      // grey level
                        {
                            /* BMP padded to 4 bytes per scan line */
                            int d = (h + 3) / 4 * 4;
                            if (d > sizeof (buf)) {
                                fprintf(stderr, "Too wide\n");
                                return false;
                            }
                            l = fread((char *)buf, 1, d, bitmap_file);
                            if (l != d) {
                                fprintf (stderr, "Bad bit data from gs %d/%d (y=%d)\n", l, d, y);
                                return false;
                            }
                            for (l = 0; l < h; l++) {
                                buf[l] = (255 - (uint8_t)buf[l]);
                            }
                        }
                        break;
                        default:       // mono
                        {
                            int d = (h + 3) / 4 * 4;  // BMP padded to 4 bytes per scan line
                            if (d > sizeof (buf))
                            {
                                perror("Too wide");
                                return false;
                            }
                            l = fread((char *) buf, 1, d, bitmap_file);
                            if (l != d)
                            {
                                fprintf(stderr, "Bad bit data from gs %d/%d (y=%d)\n", l, d, y);
                                return false;
                            }
                        }
                        }

                        if (lconf->raster_mode == 'c' || lconf->raster_mode == 'g') {
                            for (l = 0; l < h; l++) {
                                /* Raster value is multiplied by the
                                 * power scale.
                                 */
                                buf[l] = (uint8_t)buf[l] * lconf->raster_power / 255;
                            }
                        }

                        /* find left/right of data */
                        for (l = 0; l < h && !buf[l]; l++) {
                            ;
                        }

                        if (l < h) {
                            /* a line to print */
                            int r;
                            int n;
                            unsigned char pack[sizeof (buf) * 5 / 4 + 1];
                            for (r = h - 1; r > l && !buf[r]; r--) {
                                ;
                            }
                            r++;
                            fprintf(pjl_file, PCL_POS_Y, lconf->basey + offy + y);
                            fprintf(pjl_file, PCL_POS_X, lconf->basex + offx +
                                    ((lconf->raster_mode == 'c' || lconf->raster_mode == 'g') ? l : l * 8));
                            if (dir) {
                                fprintf(pjl_file, R_INTENSITY, -(r - l));
                                // reverse bytes!
                                for (n = 0; n < (r - l) / 2; n++){
                                    unsigned char t = buf[l + n];
                                    buf[l + n] = buf[r - n - 1];
                                    buf[r - n - 1] = t;
                                }
                            } else {
                                fprintf(pjl_file, R_INTENSITY, (r - l));
                            }
                            dir = 1 - dir;
                            // pack
                            n = 0;
                            while (l < r) {
                                int p;
                                for (p = l; p < r && p < l + 128 && buf[p]
                                         == buf[l]; p++) {
                                    ;
                                }
                                if (p - l >= 2) {
                                    // run length
                                    pack[n++] = 257 - (p - l);
                                    pack[n++] = buf[l];
                                    l = p;
                                } else {
                                    for (p = l;
                                         p < r && p < l + 127 &&
                                             (p + 1 == r || buf[p] !=
                                              buf[p + 1]);
                                         p++) {
                                        ;
                                    }

                                    pack[n++] = p - l - 1;
                                    while (l < p) {
                                        pack[n++] = buf[l++];
                                    }
                                }
                            }
                            fprintf(pjl_file, R_ROW_BYTES, (n + 7) / 8 * 8);
                            r = 0;
                            while (r < n)
                                fputc(pack[r++], pjl_file);
                            while (r & 7)
                            {
                                r++;
                                fputc(0x80, pjl_file);
                            }
                        }
                    }
                }
            }
        }
        fprintf(pjl_file, "\e*rC");       // end raster
        fputc(26, pjl_file);      // some end of file markers
        fputc(4, pjl_file);
    }
    return true;
}

/**
 *
 */
bool
generate_vector(printer_job *pjob, laser_config *lconf, FILE *vector_file)
{
    char up = 1;           // output status
    char firstdown=1;
    char newline = 1;      // input status (last was M)
    char started = 0;
    int sx = 0;
    int sy = 0;
    int lx = 0;
    int ly = 0;
    int power = 100;
    int offx;
    int offy;


    for (offy = lconf->height * (lconf->y_repeat - 1); offy >= 0; offy -= lconf->height) {
        for (offx = lconf->width * (lconf->x_repeat - 1); offx >= 0; offx -= lconf->width) {
            char passstart = 0;
            rewind(vector_file);
            while (fgets((char *) buf, sizeof (buf), vector_file)) {
                if (isalpha(*buf)) {
                    int x,
                        y;
                    if (!passstart) {
                        passstart = 1;
                        fprintf(pjl_file, V_INIT);
                        fprintf(pjl_file, SEP);
                        fprintf(pjl_file, V_FREQUENCY, lconf->vector_freq);
                        fprintf(pjl_file, SEP);
                        fprintf(pjl_file, V_POWER, lconf->vector_power);
                        fprintf(pjl_file, SEP);
                        fprintf(pjl_file, V_SPEED, lconf->vector_speed);
                        fprintf(pjl_file, SEP);
                    }
                    switch (*buf) {
                    case 'M': // move
                        if (sscanf((char *) buf + 1, "%d,%d", &y, &x)
                            == 2) {
                            sx = x;
                            sy = y;
                            newline = 1;
                        }
                        break;
                    case 'C': // close - only makes sense after an "L"
                        if (newline == 0 && up == 0 && (lx != sx || ly
                                                        != sy)) {
                            fprintf(pjl_file, ",%d,%d", lconf->basex + offx + sx +
                                    HPGLX, lconf->basey + offy + sy + HPGLY);
                        }
                        break;
                    case 'P': // power
                        if (sscanf((char *)buf + 1, "%d", &x) == 1
                            && x != power) {
                            int epower;
                            power = x;
                            if (!started) {
                                started = 1;
                                /* XXX disabled as current code path inserts
                                 *  this statement AFTER the IN; statement.
                                 */
                                /* start HPGL */
/*                                 fprintf(pjl_file, "\e%%1B");     */
                            }
                            if (!up) {
								if(firstdown){
									firstdown = 0;
									fprintf(pjl_file, SEP);
									fprintf(pjl_file, HPGL_PEN_UP_INIT);
								} else {
									fprintf(pjl_file, SEP);
									fprintf(pjl_file, HPGL_PEN_UP);
								}
                            }
                            up = 1;
                            epower = (power * lconf->vector_power + 50) / 100;
                            if (lconf->vector_speed && lconf->vector_speed < 100) {
                                int espeed = lconf->vector_speed;
                                int efreq = lconf->vector_freq;
                                if (epower && x < 100) {
                                    int r;
                                    int q;
                                    r = 10000 / x; // power, up to set power level (i.e. x=100)
                                    q = 10000 / espeed;
                                    if (q < r)
                                        r = q;
                                    q = 500000 / efreq;
                                    if (q < r)
                                        r = q;
                                    epower = (50 + epower * r) / 100;
                                    espeed = (50 + espeed * r) / 100;
                                    efreq = (50 + espeed * r) / 100;
                                }

                                fprintf(pjl_file, SEP);
								fprintf(pjl_file, V_SPEED, espeed);
	                            fprintf(pjl_file, SEP);
								fprintf(pjl_file, V_FREQUENCY, efreq);
	                            fprintf(pjl_file, SEP);
                            }
							fprintf(pjl_file, V_POWER, epower);
                            fprintf(pjl_file, SEP);
                        }
                        break;
                    case 'L': // line
						if (!started) {
							started = 1;
							//fprintf(pjl_file, "\e%%1B;");      // start HPGL
						}
						if (newline) {
							if (!up)
								fprintf(pjl_file, SEP);

							if (firstdown) {
								firstdown = 0;
								fprintf(pjl_file, HPGL_PEN_UP_INIT);
								fprintf(pjl_file, "%d,%d", lconf->basex + offx
										+ sx + HPGLX, lconf->basey + offy + sy + HPGLY);
							} else {
								fprintf(pjl_file, HPGL_PEN_UP);
								fprintf(pjl_file, "%d,%d", lconf->basex + offx + sx
										+ HPGLX, lconf->basey + offy + sy + HPGLY);
							}
							//fprintf(pjl_file, SEP);
							//fprintf(pjl_file, V_POWER, lconf->vector_power);
							/*if(i % 5 == 0)
							{
								if (lconf->vector_power < 100)
								{
									fprintf(pjl_file, SEP);
									fprintf(pjl_file, V_POWER, lconf->vector_power++);
								}
								else if(lconf->vector_speed > 1)
								{
									fprintf(pjl_file, SEP);
									fprintf(pjl_file, V_SPEED, lconf->vector_speed--);
								}
								else {
									lconf->vector_power = 1;
									lconf->vector_speed = 100;
								}
							}
							i++; */
							up = 1;
							newline = 0;
						}
						if (up) {
							fprintf(pjl_file, SEP);
							fprintf(pjl_file, HPGL_PEN_DOWN);
						} else {
							fprintf(pjl_file, ",");
						}
						up = 0;
						if (sscanf((char *) buf + 1, "%d,%d", &y, &x) == 2) {
							fprintf(pjl_file, "%d,%d",
									lconf->basex + offx + x + HPGLX, lconf->basey + offy + y
											+ HPGLY);

						}
						lx = x;
						ly = y;
						break;
					}

					if (*buf == 'X')
						break;
				}
            }
        }
    }
    if (started) {
        if (up == 0)
            fprintf(pjl_file, ";");
        fprintf(pjl_file, HPGL_END);      // end HLGL
    }
    fprintf(pjl_file, PCL_SECTION_END);
    fprintf(pjl_file, HPGL_PEN_UP);
    return true;
}


/**
 *
 */
bool
generate_pjl(printer_job *pjob, laser_config *lconf, FILE *bitmap_file, FILE *vector_file)
{
    int i;

    /* Print the printer job language header. */
    fprintf(pjl_file, PJL_HEADER, pjob->title->data());
    /* Set autofocus on or off. */
    fprintf(pjl_file, PCL_AUTOFOCUS, lconf->focus);
    /* FIXME unknown purpose. */
    fprintf(pjl_file, PCL_UNKNOWN_BLAFOO);
    /* FIXME unknown purpose. */
    fprintf(pjl_file, PCL_UNKNOWN_BLAFOO2);
    /* Left (long-edge) offset registration.  Adjusts the position of the
     * logical page across the width of the page.
     */
    fprintf(pjl_file, PCL_OFF_X, 0);
    /* Top (short-edge) offset registration.  Adjusts the position of the
     * logical page across the length of the page.
     */
    fprintf(pjl_file, PCL_OFF_Y, 0);
    /* Resolution of the print. */
    fprintf(pjl_file, PCL_PRINT_RESOLUTION, lconf->resolution);
    /* X position = 0 */
    fprintf(pjl_file, PCL_POS_X, 0);
    /* Y position = 0 */
    fprintf(pjl_file, PCL_POS_Y, 0);
    /* PCL resolution. */
    fprintf(pjl_file, PCL_RESOLUTION, lconf->resolution);

    /* If raster power is enabled and raster mode is not 'n' then add that
     * information to the print job.
     */
    fprintf(stderr, "##### ENTERED1");

    if (lconf->raster_power && lconf->raster_mode != 'n') {
    	fprintf(stderr, "##### ENTERED");
        /* We're going to perform a raster print. */
        generate_raster(pjob, lconf, bitmap_file);
    }

    /* If vector power is > 0 then add vector information to the print job. */
    if (lconf->vector_power) {
    	fprintf(pjl_file, R_ORIENTATION, 0);
    	fprintf(pjl_file, R_POWER, 50);
        fprintf(pjl_file, R_SPEED, 50);
        fprintf(pjl_file, PCL_UNKNOWN_BLAFOO3);
        fprintf(pjl_file, R_HEIGHT, lconf->height * lconf->y_repeat);
        fprintf(pjl_file, R_WIDTH, lconf->width * lconf->x_repeat);

    	/* seems to be obsolete, but windows driver does it*/
    	fprintf(pjl_file, R_COMPRESSION);
        fprintf(pjl_file, PCL_SECTION_END);


        /* We're going to perform a vector print. */
        generate_vector(pjob, lconf, vector_file);
    }

    fprintf(pjl_file, PCL_RESET);
    fprintf(pjl_file, PCL_EXIT);
    fprintf(pjl_file, PJL_FOOTER);

    /* Pad out the remainder of the file with 0 characters. */
    for(i = 0; i < 4096; i++) {
        fputc(0, pjl_file);
    }
    return true;
}

/**
 * Convert the given postscript file (ps) converting it to an encapsulated
 * postscript file (eps).
 *
 * @param ps_file a file handle pointing to an opened postscript file that
 * is to be converted.
 * @param eps_file a file handle pointing to the opened encapsulated
 * postscript file to store the result.
 *
 * @return Return true if the function completes its task, false otherwise.
 */
bool ps_to_eps(laser_config *lconf, FILE *ps_file, FILE *eps_file) {
	int xoffset = 0;
	int yoffset = 0;

	int l;
	while (fgets((char *) buf, sizeof(buf), ps_file)) {
		fprintf(eps_file, "%s", (char *) buf);
		if (*buf != '%') {
			break;
		}
		if (!strncasecmp((char *) buf, "%%BoundingBox:", 14)) {
			int lower_left_x;
			int lower_left_y;
			int upper_right_x;
			int upper_right_y;
			if (sscanf((char *) buf + 14, "%d %d %d %d", &lower_left_x,
					&lower_left_y, &upper_right_x, &upper_right_y) == 4) {
				xoffset = lower_left_x;
				yoffset = lower_left_y;
				lconf->width = (upper_right_x - lower_left_x);
				lconf->height = (upper_right_y - lower_left_y);
				fprintf(eps_file, "/setpagedevice{pop}def\n"); // use bbox
				fprintf(eps_file, "0 %d translate\n", lconf->height
						- upper_right_y);
				if (xoffset || yoffset) {
					fprintf(eps_file, "%d %d translate\n", -xoffset, -yoffset);
				}
				if (lconf->flip) {
					fprintf(eps_file, "%d 0 translate -1 1 scale\n",
							lconf->width);
				}
			}
		}
		if (!strncasecmp((char *) buf, "%!", 2)) {
			fprintf(
					eps_file,
					"/==={(        )cvs print}def/stroke{currentrgbcolor 0.0 \
eq exch 0.0 eq and exch 0.0 ne and{(P)=== currentrgbcolor pop pop 100 mul \
round  cvi = flattenpath{transform(M)=== round cvi ===(,)=== round cvi \
=}{transform(L)=== round cvi ===(,)=== round cvi =}{}{(C)=}pathforall \
newpath}{stroke}ifelse}bind def/showpage{(X)= showpage}bind def\n");
			if (lconf->raster_mode != 'c' && lconf->raster_mode != 'g') {
				if (lconf->screen == 0) {
					fprintf(eps_file, "{0.5 ge{1}{0}ifelse}settransfer\n");
				} else {
					int s = lconf->screen;
					if (s < 0) {
						s = 0 - s;
					}
					if (lconf->resolution >= 600) {
						// adjust for overprint
						fprintf(eps_file,
								"{dup 0 ne{%d %d div add}if}settransfer\n",
								lconf->resolution / 600, s);
					}
					fprintf(eps_file, "%d 30{%s}setscreen\n", lconf->resolution
							/ s, (lconf->screen > 0) ? "pop abs 1 exch sub"
							: "180 mul cos exch 180 mul cos add 2 div");
				}
			}
		}
	}
	while ((l = fread((char *) buf, 1, sizeof(buf), ps_file)) > 0) {
		fwrite((char *) buf, 1, l, eps_file);
	}
	return true;
}




/**
 * Process the queue options which take a form akin to:
 * @verbatim
 * /rp=100/rs=100/vp=100/vs=10/vf=5000/rm=mono/flip/af
 * @endverbatim
 * Each option is separated by the '/' character and can be specified as
 * /name=value or just /name (for boolean values).
 *
 * This function has the side effect of modifying the global variables
 * specifying major settings for the device.
 *
 * @param queue a string containing the options that are to be interpreted and
 * assigned to the global variables controlling printer characteristics.
 *
 * @return True if the function is able to complete its task, false otherwise.
 */
bool
process_uri_options(laser_config *lconf, char *queue_options)
{
    char *o = strchr(queue_options, '/');

    if (!queue_options) {
        fprintf(stderr, "URI syntax epilog://host/queue_optionsname\n");
        return false;
    }
    *queue_options++ = 0;

    if (o) {
        *o++ = 0;
        while (*o) {
            char *t = o,
                *v = "1";
            while (isalpha(*o)) {
                o++;
            }
            if (*o == '=') {
                *o++ = 0;
                v = o;
                while (*o && (isalnum(*o) || *o == '-')) {
                    o++;
                }
            }
            while (*o && !isalpha(*o)) {
                *o++ = 0;
            }
            if (!strcasecmp(t, "af")) {
                lconf->focus = atoi(v);
            }
            if (!strcasecmp(t, "r")) {
            	lconf->resolution = atoi(v);
            }
            if (!strcasecmp(t, "rs")) {
            	lconf->raster_speed = atoi(v);
            }
            if (!strcasecmp(t, "rp")) {
            	lconf->raster_power = atoi(v);
            }
            if (!strcasecmp(t, "rm")) {
            	lconf->raster_mode = tolower(*v);
            }
            if (!strcasecmp(t, "rr")) {
            	lconf->raster_repeat = atoi(v);
            }
            if (!strcasecmp(t, "vs")) {
            	lconf->vector_speed = atoi(v);
            }
            if (!strcasecmp(t, "vp")) {
            	lconf->vector_power = atoi(v);
            }
            if (!strcasecmp(t, "vf")) {
            	lconf->vector_freq = atoi(v);
            }
            if (!strcasecmp(t, "sc")) {
            	lconf->screen = atoi(v);
            }
            if (!strcasecmp(t, "w")) {
            	lconf->width = atoi(v);
            }
            if (!strcasecmp(t, "h")) {
            	lconf->height = atoi(v);
            }
            if (!strcasecmp(t, "flip")) {
            	lconf->flip = 1;
            }
            if (!strcasecmp(t, "debug")) {
            	debug = 1;
            }
        }
    }
    return true;
}
/**
 * VectorPower=98 PageSize=LaserBed Resolution=75 RasterSpeed=9 AutoFocus number-up=1 RasterPower=99 VectorMode=Engrave VectorSpeed=8 RasterMode=Color
 */
bool
process_print_job_options(printer_job *pjob, laser_config *lconf)
{
    char *o = strchr(pjob->options->data(), ' ');

    //*pjob_options++ = 0;

    if (o) {
        *o++ = 0;
        while (*o) {
            char *t = o,
                *v = "1";
            while (isalpha(*o)) {
                o++;
            }
            if (*o == '=') {
                *o++ = 0;
                v = o;
                while (*o && (isalnum(*o) || *o == '-')) {
                    o++;
                }
            }
            while (*o && !isalpha(*o)) {
                *o++ = 0;
            }
            if (!strcasecmp(t, "AutoFocus")) {
                lconf->focus = atoi(v);
            }
            if (!strcasecmp(t, "Resolution")) {
            	lconf->resolution = atoi(v);
            }
            if (!strcasecmp(t, "RasterSpeed")) {
            	lconf->raster_speed = atoi(v);
            }
            if (!strcasecmp(t, "RasterPower")) {
            	lconf->raster_power = atoi(v);
            }
            if (!strcasecmp(t, "RasterMode")) {
            	lconf->raster_mode = tolower(*v);
            	lconf->raster_mode = 'g';
            }
            if (!strcasecmp(t, "RasterRepeat")) {
            	lconf->raster_repeat = atoi(v);
            }
            if (!strcasecmp(t, "VectorSpeed")) {
            	lconf->vector_speed = atoi(v);
            }
            if (!strcasecmp(t, "VectorPower")) {
            	lconf->vector_power = atoi(v);
            }
            if (!strcasecmp(t, "VectorFrequency")) {
            	lconf->vector_freq = atoi(v);
            }
/*            if (!strcasecmp(t, "sc")) {
            	lconf->screen = atoi(v);
            }*/
/*            if (!strcasecmp(t, "w")) {
            	lconf->width = atoi(v);
            }
            if (!strcasecmp(t, "h")) {
            	lconf->height = atoi(v);
            }*/
            if (!strcasecmp(t, "FlipX")) {
            	lconf->flip = 1;
            }
            if (!strcasecmp(t, "Debug")) {
            	debug = 1;
            }
        }
    }
    return true;
}


/**
 * Perform range validation checks on the major global variables to ensure
 * their values are sane. If values are outside accepted tolerances then modify
 * them to be the correct value.
 *
 * @return Nothing
 */
void
range_checks(void)
{
    if (lconf.raster_power > 100) {
    	lconf.raster_power = 100;
    }
    if (lconf.raster_power < 0) {
    	lconf.raster_power = 0;
    }

    if (lconf.raster_speed > 100) {
    	lconf.raster_speed = 100;
    }
    if (lconf.raster_speed < 1) {
    	lconf.raster_speed = 1;
    }

    if (lconf.resolution > 1200) {
    	lconf.resolution = 1200;
    }
    if (lconf.resolution < 75) {
    	lconf.resolution = 75;
    }

    if (lconf.screen < 1) {
    	lconf.screen = 1;
    }

    if (lconf.vector_freq < 10) {
    	lconf.vector_freq = 10;
    }
    if (lconf.vector_freq > 5000) {
    	lconf.vector_freq = 5000;
    }

    if (lconf.vector_power > 100) {
    	lconf.vector_power = 100;
    }
    if (lconf.vector_power < 0) {
    	lconf.vector_power = 0;
    }

    if (lconf.vector_speed > 100) {
    	lconf.vector_speed = 100;
    }
    if (lconf.vector_speed < 1) {
    	lconf.vector_speed = 1;
    }
}



/**
 * Main entry point for the program.
 *
 * @param argc The number of command line options passed to the program.
 * @param argv An array of strings where each string represents a command line
 * argument.
 * @return An integer where 0 represents successful termination, any other
 * value represents an error code.
 */
int
main(int argc, char *argv[])
{
	init_laser_config(&lconf);
	int w = lconf.width;

	/** The printer hostname. */
    static char *host = "";

    /* Strings designating filenames. */
    char file_basename[FILENAME_NCHARS];
    char filename_bitmap[FILENAME_NCHARS];
    char filename_cups_debug[FILENAME_NCHARS];
    char filename_eps[FILENAME_NCHARS];
    char filename_pdf[FILENAME_NCHARS];
    char filename_pjl[FILENAME_NCHARS];
    char filename_ps[FILENAME_NCHARS];
    char filename_vector[FILENAME_NCHARS];

    /* File handles. */
    FILE *file_debug;
    FILE *file_bitmap;
    FILE *file_eps;
    FILE *file_cups;
    FILE *file_pdf;
    FILE *file_ps;
    FILE *file_ps_output;
    FILE *file_vector;
    FILE *file_template;

    /* Temporary variables. */
    int l;

    /*
     * Process the command line arguments specified by the user. Proper command
     * line arguments are fed to this program from cups and will be of a form akin
     * to:
     * @verbatim
     * job_number user job_title num_copies queue_options
     * 123        jdoe myjob     1          asdf
     * @endverbatim
     */
    if (argc == 1) {
        printf("direct epilog \"Unknown\" \"Epilog laser (thin red lines vector cut)\"\n");
        return 1;
    }
    if (argc > 1) {
    	pjob.name = new string(argv[1]);
    }
    if (argc > 2) {
    	pjob.user = new string(argv[2]);
    }
    if (argc > 3) {
    	pjob.title = new string(argv[3]);
    }
    if (argc > 4) {
    	pjob.copies = new string(argv[4]);
    }
    if (argc > 5) {
    	pjob.options = new string(argv[5]);
    }else
    	pjob.options = new string();
    /* Gather the site information from the user's environment variable. */
    device_uri = getenv("DEVICE_URI");
    if (!device_uri) {
        fprintf(stderr, "No $DEVICE_URI set\n");
        return 0;
    }
    host = strstr(device_uri, "//");
    if (!host) {
        fprintf(stderr, "URI syntax epilog://host/queuename\n");
        return 0;
    }
    host += 2;

    /* Process the queue arguments. */
    queue_options = strchr(host, '/');
    if (!process_uri_options(&lconf, queue_options)) {
        fprintf(stderr, "Error processing epilog uri options.");
        return 0;
    }

    if (argc > 5 && !process_print_job_options(&pjob, &lconf)) {
        fprintf(stderr, "Error processing epilog printer options.");
        return 0;
    }

    /* Perform a check over the global values to ensure that they have values
     * that are within a tolerated range.
     */
    range_checks();

    /* Determine and set the names of all files that will be manipulated by the
     * program.
     */
    sprintf(file_basename, "%s/%s-%d", TMP_DIRECTORY, FILE_BASENAME, getpid());
    sprintf(filename_bitmap, "%s.bmp", file_basename);
    sprintf(filename_eps, "%s.eps", file_basename);
    sprintf(filename_pjl, "%s.pjl", file_basename);
    sprintf(filename_vector, "%s.vector", file_basename);
    pjob.pjl_filename = new string(filename_pjl);

    /* Gather the postscript file from either standard input or a filename
     * specified as a command line argument.
     */
    if (argc > 6) {
        file_cups = fopen(argv[6], "r");
    } else {
        file_cups = stdin;
    }
    if (!file_cups) {
        perror((argc > 6) ? argv[6] : "stdin");
        return 1;
    }

    /* Write out the incoming cups data if debug is enabled. */
    if (debug) {
        /* We save the incoming cups data to the filesystem. */
        sprintf(filename_cups_debug, "%s.cups", file_basename);
        file_debug = fopen(filename_cups_debug, "w");

        /* Check that file handle opened. */
        if (!file_debug) {
            perror(filename_cups_debug);
            return 1;
        }

        /* Write cups data to the filesystem. */
        while ((l = fread((char *)buf, 1, sizeof(buf), file_cups)) > 0) {
            fwrite((char *)buf, 1, l, file_debug);
        }
        fclose(file_debug);
        /* In case file_cups pointed to stdin we close the existing file handle
         * and switch over to using the debug file handle.
         */
        fclose(file_cups);
        file_cups = fopen(filename_cups_debug, "r");
    }

/*    if(argc < 6 || strcasecmp(argv[5], "=A4") == 0)
    {
    	fprintf(stderr, "########## GTKLP");
    	if(!execute_gtklp(PRINTER_NAME, filename_cups_debug, GTKLP_CONF_DIR)) {
			perror("Failure to gtklp ghostscript command.\n");
			return 1;
		}
    }*/

    /* Check whether the incoming data is ps or pdf data. */
    fread((char *)buf, 1, 4, file_cups);
    rewind(file_cups);
    if (strncasecmp((char *)buf, "%PDF", 4) == 0) {
        /* We have a pdf file. Convert it to postscript. */
        sprintf(filename_pdf, "%s.pdf", file_basename);
        sprintf(filename_ps, "%s.ps", file_basename);

        file_pdf = fopen(filename_pdf, "w");
        if (!file_pdf) {
            perror(filename_pdf);
            return 1;
        }

        /* Write the cups data out to the file_pdf. */
        while ((l = fread((char *)buf, 1, sizeof(buf), file_cups)) > 0) {
            fwrite((char *)buf, 1, l, file_pdf);
        }

        fclose(file_cups);
        fclose(file_pdf);

        /* Execute the command pdf2ps to convert the pdf file to ps. */
        sprintf(buf, "/usr/bin/pdf2ps %s %s", filename_pdf, filename_ps);
        if (debug) {
            fprintf(stderr, "%s\n", buf);
        }
        if (system(buf)) {
            fprintf(stderr, "Failure to execute pdf2ps. Quitting...");
            return 1;
        }

        if (!debug) {
            /* Debug is disabled so remove generated pdf file. */
            if (unlink(filename_pdf)) {
                perror(filename_pdf);
            }
        }

        /* Set file_ps to the generated ps file. */
        file_ps  = fopen(filename_ps, "r");
    } else {

        /* Input file is postscript. Set the file_ps handle to file_cups. */
        file_ps = file_cups;
    }

    /* Open the encapsulated postscript file for writing. */
    file_eps = fopen(filename_eps, "w");
    if (!file_eps) {
        perror(filename_eps);
        return 1;
    }
    /* Convert postscript to encapsulated postscript. */
    if (!ps_to_eps(&lconf, file_ps, file_eps)) {
        perror("Error converting postscript to encapsulated postscript.");
        fclose(file_eps);
        return 1;
    }
    /* Cleanup after encapsulated postscript creation. */
    fclose(file_eps);
    if (file_ps != stdin) {
        fclose(file_ps);
        if (unlink(filename_ps)) {
            perror(filename_ps);
        }
    }

    string* rm;

    if(lconf.raster_mode == 'c')
    	rm = new string("bmp16m");
    else if(lconf.raster_mode == 'g')
    	rm = new string("bmpgray");
    else
    	rm = new string("bmpmono");

    if(!execute_ghostscript(filename_bitmap,
                            filename_eps,
                            filename_vector,
							(char *)rm->data(),
                            		lconf.resolution, lconf.height, lconf.width)) {
        perror("Failure to execute ghostscript command.\n");
        return 1;
    }

    /* Open file handles needed by generation of the printer job language
     * file.
     */
    file_bitmap = fopen(filename_bitmap, "r");

    pjl_file = fopen(filename_pjl, "w");
    if (!pjl_file) {
        perror(filename_pjl);
        return 1;
    }
	optimize_vectors(filename_vector, ((lconf.width * lconf.resolution)
			/ POINTS_PER_INCH) / 2, ((lconf.height * lconf.resolution)
			/ POINTS_PER_INCH) / 2);
	//return 0;
	file_vector = fopen(filename_vector, "r");
	/* Execute the generation of the printer job language (pjl) file. */
    if (!generate_pjl(&pjob, &lconf, file_bitmap, file_vector)) {
        perror("Generation of pjl file failed.\n");
        fclose(pjl_file);
        return 1;
    }
    /* Close open file handles. */
    fclose(file_bitmap);
    fclose(file_vector);
    fclose(pjl_file);

    /* Cleanup unneeded files provided that debug mode is disabled. */
    if (!debug) {
        if (unlink(filename_bitmap)) {
            perror(filename_bitmap);
        }
        if (unlink(filename_eps)) {
            perror(filename_eps);
        }
        if (unlink(filename_vector)) {
            perror(filename_vector);
        }
    }

    /* Open printer job language file. */
    pjl_file = fopen(filename_pjl, "r");
    if (!pjl_file) {
        perror(filename_pjl);
        return 1;
    }
    /* Send print job to printer. */

    PrinterConnection pc(new string(host), PRINTER_MAX_WAIT);

    if (!pc.connect() || !pc.send(&pjob)) {
        perror("Could not send pjl file to printer.\n");
        return 1;
    }
    fclose(pjl_file);
    if (!debug) {
        if (unlink(filename_pjl)) {
            perror(filename_pjl);
        }
    }

    return 0;
}

