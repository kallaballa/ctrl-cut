/*
 * EpilogCUPS - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <time.h>

#include <cups/cups.h>

#include "util/LaserConfig.h"
#include "util/Eps.h"
#include "vector/Cut.h"
#include "LaserJob.h"
#include "Driver.h"
#include "EpilogFilter.h"

#ifndef __APPLE__
#define GS_EXECUTABLE "/usr/bin/gs"
#else
#define GS_EXECUTABLE "/opt/local/bin/gs"
#endif

/** The laser cutter configuration **/
LaserConfig lconf;

/** Temporary buffer for building our strings. */
char buf[102400];

/** Determines whether or not debug is enabled. */
char debug = DEBUG;

const char *queue_options = "";

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
bool execute_ghostscript(char *filename_bitmap, char *filename_eps,
		char *filename_vector, const char *bmp_mode, int resolution, int height,
		int width) {

	char buf[8192];
	sprintf(
			buf,
			"%s -q -dBATCH -dNOPAUSE -r%d -g%dx%d -sDEVICE=%s -sOutputFile=%s %s > %s",
			GS_EXECUTABLE, resolution, (width * resolution) / POINTS_PER_INCH,
			(height * resolution) / POINTS_PER_INCH, bmp_mode, filename_bitmap,
			filename_eps, filename_vector);

	if (debug) {
		fprintf(stderr, "DEBUG: %s\n", buf);
	}
	if (system(buf)) {
		return false;
	}
	return true;
}

/*!
 Copy supported options into the supplied laser_config:

 VectorPower=98
 PageSize=LaserBed
 Resolution=75
 RasterSpeed=9
 AutoFocus
 number-up=1
 RasterPower=99
 VectorMode=Engrave
 VectorSpeed=8
 RasterMode=Color
 */
void process_print_job_options(cups_option_t *options, int numOptions,
		LaserConfig *lconf) {
	const char *v;
	if ((v = cupsGetOption("AutoFocus", numOptions, options))) {
		lconf->focus = atoi(v);
	}
	if ((v = cupsGetOption("Resolution", numOptions, options))) {
		lconf->resolution = atoi(v);
	}
	if ((v = cupsGetOption("RasterSpeed", numOptions, options))) {
		lconf->raster_speed = atoi(v);
	}
	if ((v = cupsGetOption("RasterPower", numOptions, options))) {
		lconf->raster_power = atoi(v);
	}
	if ((v = cupsGetOption("RasterMode", numOptions, options))) {
		lconf->raster_mode = tolower(*v);
		lconf->raster_mode = 'g';
	}
	if ((v = cupsGetOption("RasterRepeat", numOptions, options))) {
		lconf->raster_repeat = atoi(v);
	}
	if ((v = cupsGetOption("VectorSpeed", numOptions, options))) {
		lconf->vector_speed = atoi(v);
	}
	if ((v = cupsGetOption("VectorPower", numOptions, options))) {
		lconf->vector_power = atoi(v);
	}
	if ((v = cupsGetOption("VectorFrequency", numOptions, options))) {
		lconf->vector_freq = atoi(v);
	}
	if ((v = cupsGetOption("FlipX", numOptions, options))) {
		lconf->flip = 1;
	}
	if ((v = cupsGetOption("Debug", numOptions, options))) {
		debug = 1;
	}
}

/**
 * Perform range validation checks on the major global variables to ensure
 * their values are sane. If values are outside accepted tolerances then modify
 * them to be the correct value.
 *
 * @return Nothing
 */
void range_checks(void) {
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

static void printEnv(const char *env) {
	const char *envstr = getenv(env);
	if (envstr) {
		fprintf(stderr, "DEBUG: %s = %s\n", env, envstr);
	}
}


void ppm2pcl(string filename) {
	Raster *raster = Raster::load(filename);
	raster->addTile(raster->sourceImage);
	lconf.raster_repeat = 1;
	PclRenderer renderer(&lconf);
	renderer.renderRaster(raster, cout);
	exit(0);
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

int main(int argc, char *argv[]) {
	clock_t start = clock();

	// Make sure status messages are not buffered
	setbuf(stderr, NULL);

	if (argc < 6 || argc > 7) {
		fprintf(stderr,
				"ERROR: Usage: %s job-id user title copies options [file]\n", argv[0]);
		return 1;
	}

	// Arguments debug output:
	fprintf(stderr, "DEBUG: ");
	for (int i = 0; i < argc; i++) {
		fprintf(stderr, "%s ", argv[i]);
	}
	fprintf(stderr, "\n");

	// Env. variable debug output
	printEnv("CHARSET");
	printEnv("CLASS");
	printEnv("CONTENT_TYPE");
	printEnv("CUPS_CACHEDIR");
	printEnv("CUPS_DATADIR");
	printEnv("CUPS_FILETYPE");
	printEnv("CUPS_SERVERROOT");
	printEnv("DEVICE_URI");
	printEnv("FINAL_CONTENT_TYPE");
	printEnv("LANG");
	printEnv("PATH");
	printEnv("PPD");
	printEnv("PRINTER");
	printEnv("RIP_CACHE");
	printEnv("SOFTWARE");
	printEnv("TZ");
	printEnv("USER");

	// FIXME: Register a signal handler to support cancelling of jobs

	cups_file_t *fp;
	if (argc == 6) {
		fp = cupsFileStdin();
	} else {
		// Try to open the print file...
		if ((fp = cupsFileOpen(argv[6], "r")) == NULL) {
			perror("ERROR: unable to open print file - ");
			return 1;
		}
	}

	const char *arg_jobid = argv[1];
	const char *arg_user = argv[2];
	const char *arg_title = argv[3];
	const char *arg_copies = argv[4];
	const char *arg_options = argv[5];

	cups_option_t *options;
	int numOptions = cupsParseOptions(arg_options, 0, &options);

	init_laser_config(&lconf);

	/* Strings designating filenames. */
	char file_basename[FILENAME_NCHARS];
	char filename_bitmap[FILENAME_NCHARS];
	char filename_cups_debug[FILENAME_NCHARS];
	char filename_eps[FILENAME_NCHARS];
	char filename_vector[FILENAME_NCHARS];

	/* File handles. */
	FILE *file_debug;
	FILE *file_bitmap;
	FILE *file_eps;
	FILE *file_cups;

	process_print_job_options(options, numOptions, &lconf);

	/* Perform a check over the global values to ensure that they have values
	 * that are within a tolerated range.
	 */
	range_checks();

	/* Determine and set the names of all files that will be manipulated by the
	 * program.
	 */
	sprintf(file_basename, "%s/%s-%d", TMP_DIRECTORY, FILE_BASENAME, getpid());
	sprintf(filename_bitmap, "%s.ppm", file_basename);
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
		// FIXME: Prefix with ERROR:
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
			// FIXME: Prefix with ERROR:
			perror(filename_cups_debug);
			return 1;
		}

		/* Write cups data to the filesystem. */
		int l;
		while ((l = fread(buf, 1, sizeof(buf), file_cups)) > 0) {
			fwrite(buf, 1, l, file_debug);
		}
		fclose(file_debug);
		/* In case file_cups pointed to stdin we close the existing file handle
		 * and switch over to using the debug file handle.
		 */
		fclose(file_cups);
		file_cups = fopen(filename_cups_debug, "r");
	}

	/* Open the encapsulated postscript file for writing. */
	file_eps = fopen(filename_eps, "w");
	if (!file_eps) {
		// FIXME: Prefix with ERROR:
		perror(filename_eps);
		return 1;
	}

	/* Convert PS to EPS (for vector extraction) */
	if (!ps_to_eps(&lconf, file_cups, file_eps)) {
		perror("ERROR: ps_to_eps failed - ");
		fclose(file_eps);
		return 1;
	}
	/* Cleanup after encapsulated postscript creation. */
	fclose(file_eps);
	if (file_cups != stdin) {
		fclose(file_cups);
	}

	const char *rm;

	/*  if (lconf.raster_mode == 'c')
	 rm = "bmp16m";
	 else if (lconf.raster_mode == 'g')
	 rm = "bmpgray";
	 else
	 rm = "bmpmono";*/

	// FIXME: While doing vector testing, set this to nullpage to speed up the gs run
	//rm = "nullpage";
	rm = "ppmraw";

	if (!execute_ghostscript(filename_bitmap, filename_eps, filename_vector, rm,
			lconf.resolution, lconf.height, lconf.width)) {
		perror("ERROR: ghostscript failed -");
		return 1;
	}

	Cut *cut = Cut::load(filename_vector);
	Raster *raster = Raster::load(filename_bitmap);
	raster->addTile(raster->sourceImage);
	LaserJob job(&lconf, arg_user, arg_jobid, arg_title);
  job.addCut(cut);
	job.addRaster(raster);

	/* Cleanup unneeded files provided that debug mode is disabled. */
	if (!debug) {
		if (unlink(filename_bitmap)) {
			// FIXME: Prefix with ERROR:
			perror(filename_bitmap);
		}
		if (unlink(filename_eps)) {
			// FIXME: Prefix with ERROR:
			perror(filename_eps);
		}
		if (unlink(filename_vector)) {
			// FIXME: Prefix with ERROR:
			perror(filename_vector);
		}
	}

	Driver drv;
	drv.process(&job);

	delete cut;
	clock_t end = clock() - start;
	cerr << endl << "Clocks: " << end << endl;
	cerr << "Seconds: " << 1.0 * end / CLOCKS_PER_SEC << endl;
	return 0;
}
