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
#include <libgen.h>

#include "util/Logger.h"
#include "util/LaserConfig.h"
#include "util/Eps.h"
#include "vector/Cut.h"
#include "LaserJob.h"
#include "Driver.h"
#include "Ctrl-Cut.h"

#include <cups/cups.h>
#include <cups/file.h>

#include "FileParser.h"


/** The laser cutter configuration **/
LaserConfig lconf;
LogLevel cc_loglevel = CC_WARNING;

/** Temporary buffer for building our strings. */
char buf[102400];

const char *queue_options = "";

static void printEnv(const char *env) {
  const char *envstr = getenv(env);
  if (envstr) {
    LOG_DEBUG_MSG(env, envstr);
  }
}

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

  // Extract non-CUPS cmd-line parameters
  bool dumpxml = false;
  int c;
  while ((c = getopt(argc, argv, "x")) != -1) {
    switch (c) {
    case 'x':
      dumpxml = true;
      break;
    case ':':
      printUsage(argv[0]);
      break;
    case '?':
      printUsage(argv[0]);
      break;
    }
  }
  /** FIXME: free of cwd impairs logger???
  char *cwd = getwd(NULL);
  LOG_DEBUG(cwd);
  free(cwd);
  */

  // Now, only standard CUPS parameters should be left
  int cupsargs = argc - optind;
  if (cupsargs < 5 || cupsargs > 6) {
    printUsage(argv[0]);
  }

  for (int i = optind; i < argc; i++) {
    LOG_DEBUG(argv[i]);
  }

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
  printEnv("PPD");
  printEnv("PRINTER");
  printEnv("RIP_CACHE");
  // FIXME: Use the TMPDIR to set LaserConfig::tempdir
  printEnv("TMPDIR");
  printEnv("PATH");

  // FIXME: Register a signal handler to support cancelling of jobs
  const char *arg_jobid = argv[optind];
  const char *arg_user = argv[optind + 1];
  const char *arg_title = argv[optind + 2];
  const char *arg_copies = argv[optind + 3];
  const char *arg_options = argv[optind + 4];
  const char *arg_filename = (cupsargs == 6) ? argv[optind + 5] : NULL;

  // Handle CUPS options
  cups_option_t *options;
  int numOptions = cupsParseOptions(arg_options, 0, &options);
  lconf.setCupsOptions(options, numOptions);
  // Perform a check over the global values to ensure that they have values
  // that are within a tolerated range.
  lconf.rangeCheck();

  LaserJob job(&lconf, arg_user, arg_jobid, arg_title);

  string filename_vector;
  string filename_pbm;

  enum { FORMAT_POSTSCRIPT, FORMAT_VECTOR, FORMAT_PBM } inputformat = FORMAT_POSTSCRIPT;

  cups_file_t *input_file;
  bool input_is_stdin = false;
  if (cupsargs == 5) {
    input_file = cupsFileStdin();
    input_is_stdin = true;
    lconf.datadir = "/tmp";
    lconf.basename = "stdin";
  } else {
    lconf.datadir = dirname(strdup(arg_filename));
    string base = basename(strdup(arg_filename));
    
    string suffix = base.substr(base.rfind(".") + 1);
    lconf.basename = base.erase(base.rfind("."));
    
    if (suffix == "vector") {
      inputformat = FORMAT_VECTOR;
      filename_vector = arg_filename;
    }
    else if (suffix == "pbm") {
      inputformat = FORMAT_PBM;
      filename_pbm = arg_filename;
    }
    else {
      // Try to open the print file...
      if ((input_file = cupsFileOpen(arg_filename, "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", arg_filename);
        return 1;
      }
    }
  }

  FileParser *parser = NULL;

  if (inputformat == FORMAT_POSTSCRIPT) {
    string file_basename = lconf.tempdir + "/" + lconf.basename.c_str();
    
    // Write out the incoming cups data if debug is enabled.
    // FIXME: This is disabled for now since it has a bug:
    // If we're reading from network/stdin, and debug is on, we reopen
    // the dumped file as a FILE*. Otherwise, we'll keep the cups_file_t.
    // Subsequent code doesn't handle the difference.
#if 0
    FILE *file_debug;
    FILE *file_cups;
    string filename_cups_debug;
    if (cc_loglevel >= CC_DEBUG) {
      /* We save the incoming cups data to the filesystem. */
      filename_cups_debug = file_basename + ".cups";
      file_debug = fopen(filename_cups_debug.c_str(), "w");
      
      /* Check that file handle opened. */
      if (!file_debug) {
        LOG_FATAL_MSG("Can't open", filename_cups_debug);
        return 1;
      }
      
      /* Write cups data to the filesystem. */
      int l;
      while ((l = cupsFileRead(input_file, buf, sizeof(buf))) > 0) {
        fwrite(buf, 1, l, file_debug);
      }
      fclose(file_debug);
      /* In case file_cups pointed to stdin we close the existing file handle
       * and switch over to using the debug file handle.
       */
      cupsFileClose(input_file);
      file_cups = fopen(filename_cups_debug, "r");
    }
#endif

    PostscriptParser *psparser = new PostscriptParser(lconf);
    // Uncomment this to force ghostscript to render to file using the ppmraw
    // backend, instead of in-memory rendering
    //    psparser->setRenderToFile(true);
    switch (lconf.raster_dithering) {
    case LaserConfig::DITHER_DEFAULT:
      psparser->setRasterFormat(PostscriptParser::BITMAP);
      break;
    case LaserConfig::DITHER_THRESHOLD:
    case LaserConfig::DITHER_BAYER:
    case LaserConfig::DITHER_FLOYD_STEINBERG:
    case LaserConfig::DITHER_FLOYD_JARVIS:
    case LaserConfig::DITHER_FLOYD_BURKE:
    case LaserConfig::DITHER_FLOYD_STUCKI:
    case LaserConfig::DITHER_FLOYD_SIERRA2:
    case LaserConfig::DITHER_FLOYD_SIERRA3:
      psparser->setRasterFormat(PostscriptParser::GRAYSCALE);
      break;

    default:
      assert(false);
    }
    if (!psparser->parse(input_file)) {
      LOG_FATAL("Error processing postscript");
      return 1;
    }
    else {
      parser = psparser;
    }
  }

  if (lconf.enable_raster) {
    Raster *raster = NULL;
    if (inputformat == FORMAT_PBM) {
      raster = Raster::load(filename_pbm);
    }
    else if (parser) {
      if (parser->hasBitmapData()) {
        LOG_DEBUG_STR("Processing bitmap data from memory");
        raster = new Raster(parser->getImage());
      }
      else if (!parser->getBitmapFile().empty()) {
        raster = Raster::load(parser->getBitmapFile());
      }
      else {
        LOG_FATAL("No bitmap available from FileParser");
        return 1;
      }
    }
    if (raster) {
      raster->addTile(raster->sourceImage);
      job.addRaster(raster);
    }
  }


  Cut *cut = NULL;
  if (lconf.enable_vector) {
    if (inputformat == FORMAT_VECTOR) {
      cut = Cut::load(filename_vector);
    }
    else if (parser) {
      cut = Cut::load(parser->getVectorData());
    }
    if (cut) job.addCut(cut);
  }

  Driver drv;
  if (dumpxml) drv.enableXML(true);
  drv.process(&job);

  delete cut;
  delete parser;

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
