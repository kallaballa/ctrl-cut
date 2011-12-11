/*
 * Ctrl-Cut - A laser cutter CUPS driver
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
 * GNU General Public License for more de0tails.
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
#include <libgen.h>

#include "util/Logger.h"
#include "Document.h"
#include "config/CtrlCutOptions.h"
#include "Driver.h"
#include "Ctrl-Cut.h"

/** The laser cutter configuration **/
LogLevel cc_loglevel = CC_WARNING;

/** Temporary buffer for building our strings. */
char buf[102400];

const char *queue_options = "";

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

void printUsage(const char *name) {
  fprintf(stderr, "%s %s\n", name, TOSTRING(CTRLCUT_VERSION));
  fprintf(stderr,
      "Usage: %s [options] job-id user title copies options [file]\n\n", name);

  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -x        Output xml files for debugging\n");
  fprintf(stderr, "  -f <file> Read vectors from the given file instead of the postscript\n");
  exit(1);
}

int lower_case ( int c )
{
  return tolower ( c );
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
  LOG_INFO_MSG("log level", cc_loglevel);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document job;
  CtrlCutOptions options;
  job.settings.resetToDefaults();
  options.parseGetOpt(job.settings, argc, argv);

  string filename_vector;
  string filename_pbm;

  job.load(options.filename);

  Driver drv;
  if (options.dumpXML) drv.enableXML(true);
  std::stringstream ss;
  drv.process(&job, ss);
  LOG_DEBUG_MSG("Output size", ss.str().size());
  std::cout << ss.rdbuf();

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
