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

#include <ctype.h>
#include <errno.h>
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
#include <fstream>

#include "CuttingOptimizer.h"
#include "Driver.h"
#include "CupsParser.h"


/** The printer job **/
printer_config pconf;

/** The laser cutter configuration **/
laser_config lconf;

/** Temporary buffer for building out strings. */
char buf[102400];

/** Determines whether or not debug is enabled. */
char debug = DEBUG;

/** The DEVICE_URI for the printer. */
char *device_uri = "";

char *queue_options = "";



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
process_print_job_options(printer_config *pjob, laser_config *lconf)
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



VectorPass loadVectorPass(char *file_name) {
	VectorPass vpass;
	string line;
	ifstream infile(file_name, ios_base::in);
	char first;
	int power, x, y;
	int lx, ly;
	int mx, my;
	Joint *start;
	Joint *end;

	while (getline(infile, line, '\n')) {
		first = *line.begin();

		if (first == 'X')
			break;

		if (isalpha(first)) {
			switch (first) {
			case 'M': // move
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					lx = x;
					ly = y;
					mx = x;
					my = y;
				}
				break;
			case 'C': // close
				if (lx != mx || ly != my) {
					vpass.addLine(new Joint(lx, ly), new Joint(mx, my), power);
				}
				break;
			case 'P': // power
				if (sscanf((char *) line.data() + 1, "%d", &x) == 1) {
					power = x;
				}
				break;
			case 'L': // line
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					start = new Joint(lx, ly);
					end = new Joint(x, y);
					vpass.addLine(start, end, power);
					lx = x;
					ly = y;
				}
				break;
			}
		}
	}
	infile.close();

	return vpass;
}

void calculate_base_position(laser_config *lconf)
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

void init_laser_config(laser_config *lconf)
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
    char filename_ps[FILENAME_NCHARS];
    char filename_vector[FILENAME_NCHARS];

    /* File handles. */
    FILE *file_debug;
    FILE *file_bitmap;
    FILE *file_eps;
    FILE *file_cups;
    FILE *file_pdf;
    FILE *file_ps;
    FILE *file_vector;

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
    	pconf.name = new string(argv[1]);
    }
    if (argc > 2) {
    	pconf.user = new string(argv[2]);
    }
    if (argc > 3) {
    	pconf.title = new string(argv[3]);
    }
    if (argc > 4) {
    	pconf.copies = new string(argv[4]);
    }
    if (argc > 5) {
    	pconf.options = new string(argv[5]);
    }else
    	pconf.options = new string();


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

    if (argc > 5 && !process_print_job_options(&pconf, &lconf)) {
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
    sprintf(filename_vector, "%s.vector", file_basename);

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

    VectorPass vpass = loadVectorPass(filename_vector);
    //Raster ignored at the moment
    RasterPass rpass;
    LaserJob job(&lconf,&pconf);
    job.addPass(&vpass);
    job.addPass(&rpass);

    /* Close open file handles. */
    fclose(file_bitmap);
    //fclose(file_vector);

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

    Driver drv;
    drv.process(&job);

    return 0;
}

