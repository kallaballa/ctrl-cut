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

#ifdef USE_GHOSTSCRIPT_API
#include <sstream>
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>
#include "boost/format.hpp"
using boost::str;
using boost::format;
#else
#ifndef __APPLE__
#define GS_EXECUTABLE "/usr/bin/gs"
#else
#define GS_EXECUTABLE "/opt/local/bin/gs"
#endif
#endif


/** The laser cutter configuration **/
LaserConfig lconf;
LogLevel cc_loglevel = CC_WARNING;

/** Temporary buffer for building our strings. */
char buf[102400];

const char *queue_options = "";

#ifdef USE_GHOSTSCRIPT_API
std::stringstream vectorbuffer;
static int GSDLLCALL
gsdll_stdout(void *, const char *str, int len)
{
  vectorbuffer.write(str, len);
  return len;
}
#endif

/**
 * Execute ghostscript feeding it an ecapsulated postscript file which
 * is then converted into a bitmap image. As a byproduct, output of
 * the ghostscript process is redirected to the global stringstream
 * vectorbuffer, which will contain instructions on how to perform a
 * vector cut of lines within the postscript.
 *
 * @param filename_bitmap the filename to use for the resulting bitmap file.
 * @param filename_eps the filename to read in encapsulated postscript from.
 * @param bmp_mode a string which is one of bmp16m, bmpgray, or bmpmono.
 * @param resolution the encapsulated postscript resolution.
 * @param height the postscript height in points per inch.
 * @param width the postscript width in points per inch.
 *
 * @return Return true if the execution of ghostscript succeeds, false
 * otherwise.
 */
#ifdef USE_GHOSTSCRIPT_API
bool execute_ghostscript(char *filename_eps, char *filename_bitmap,
                         const char *bmp_mode, int resolution, int height,
                         int width) {
  std::vector<std::string> argstrings;
  argstrings.push_back("gs");
  argstrings.push_back("-q");
  argstrings.push_back("-dBATCH");
  argstrings.push_back("-dNOPAUSE");
  argstrings.push_back(str(format("-r%d") % resolution));
  argstrings.push_back(str(format("-g%dx%d")
                                  % ((width * resolution) / POINTS_PER_INCH)
                                  % ((height * resolution) / POINTS_PER_INCH)));
  argstrings.push_back(str(format("-sDEVICE=%s") % bmp_mode));
  argstrings.push_back(str(format("-sOutputFile=%s") % filename_bitmap));
  argstrings.push_back(filename_eps);

  int gsargc = argstrings.size();
  const char *gsargv[gsargc];
  for (int i=0;i<gsargc;i++) {
    gsargv[i] = argstrings[i].c_str();
  }
  void *minst;
  int code = gsapi_new_instance(&minst, NULL);
  if (code < 0) {
    LOG_ERR_MSG("gsapi_new_instance() failed", code);
    return false;
  }
  gsapi_set_stdio(minst, NULL, gsdll_stdout, NULL);
  code = gsapi_init_with_args(minst, gsargc, (char **)gsargv);
  int code1 = gsapi_exit(minst);
  if ((code == 0) || (code == e_Quit)) {
    code = code1;
  }

  gsapi_delete_instance(minst);

  if ((code == 0) || (code == e_Quit)) {
    return true;
  }
  return false;
}
#endif

#ifndef USE_GHOSTSCRIPT_API
bool execute_ghostscript_cmd(char *filename_eps, char *filename_bitmap,
                             char *filename_vector, const char *bmp_mode, int resolution,
                             int height, int width) {
  char buf[8192];
  sprintf(
      buf,
      "%s -q -dBATCH -dNOPAUSE -r%d -g%dx%d -sDEVICE=%s -sOutputFile=%s %s > %s",
      GS_EXECUTABLE, resolution, (width * resolution) / POINTS_PER_INCH,
      (height * resolution) / POINTS_PER_INCH, bmp_mode, filename_bitmap,
      filename_eps, filename_vector);

  LOG_DEBUG(buf);

  if (system(buf)) {
    return false;
  }

  return true;
}
#endif

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
  char *vectorfile = NULL;
  int c;
  while ((c = getopt(argc, argv, "xf:")) != -1) {
    switch (c) {
    case 'x':
      dumpxml = true;
      break;
    case 'f':
      vectorfile = optarg;
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
  printEnv("TMPDIR");
  printEnv("PATH");

  // FIXME: Register a signal handler to support cancelling of jobs
  const char *arg_jobid = argv[optind];
  const char *arg_user = argv[optind + 1];
  const char *arg_title = argv[optind + 2];
  const char *arg_copies = argv[optind + 3];
  const char *arg_options = argv[optind + 4];
  const char *arg_filename = argv[optind + 5];

  // Handle CUPS options
  cups_option_t *options;
  int numOptions = cupsParseOptions(arg_options, 0, &options);
  lconf.setCupsOptions(options, numOptions);
  // Perform a check over the global values to ensure that they have values
  // that are within a tolerated range.
  lconf.rangeCheck();
  const char *rastermode = lconf.enable_raster ? "ppmraw" : "nullpage";

  LaserJob job(&lconf, arg_user, arg_jobid, arg_title);

  // Strings designating filenames.
  char file_basename[FILENAME_NCHARS];
  char filename_bitmap[FILENAME_NCHARS];
  char filename_eps[FILENAME_NCHARS];

  // If no vector file is given read from postscript file or stdin
  if (!vectorfile) {
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
      lconf.basename = base.erase(base.rfind("."));
      
      // Try to open the print file...
      if ((input_file = cupsFileOpen(arg_filename, "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", arg_filename);
        return 1;
      }
    }
    
    /* Determine and set the names of all files that will be manipulated by the
     * program.
     */
    sprintf(file_basename, "%s/%s", TMP_DIRECTORY, FILE_BASENAME);
    sprintf(filename_bitmap, "%s.ppm", file_basename);
    sprintf(filename_eps, "%s.eps", file_basename);
#ifndef USE_GHOSTSCRIPT_API
    char filename_vector[FILENAME_NCHARS];
    sprintf(filename_vector, "%s.vector", file_basename);
#endif
    
    // Write out the incoming cups data if debug is enabled.
    // FIXME: This is disabled for now since it has a bug:
    // If we're reading from network/stdin, and debug is on, we reopen
    // the dumped file as a FILE*. Otherwise, we'll keep the cups_file_t.
    // Subsequent code doesn't handle the difference.
#if 0
    FILE *file_debug;
    FILE *file_cups;
    char filename_cups_debug[FILENAME_NCHARS];
    if (cc_loglevel >= CC_DEBUG) {
      /* We save the incoming cups data to the filesystem. */
      sprintf(filename_cups_debug, "%s.cups", file_basename);
      file_debug = fopen(filename_cups_debug, "w");
      
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
    
    /* Open the encapsulated postscript file for writing. */
    FILE *file_eps = fopen(filename_eps, "w");
    if (!file_eps) {
      LOG_FATAL_MSG("Can't open", filename_eps);
      return 1;
    }
    LOG_DEBUG_MSG("Converting to eps file", filename_eps);
    
    /* Convert PS to EPS (for vector extraction) */
    if (!ps_to_eps(&lconf, input_file, file_eps)) {
      LOG_FATAL_STR("ps_to_eps failed");
      fclose(file_eps);
      return 1;
    }
    /* Cleanup after encapsulated postscript creation. */
    fclose(file_eps);
    if (!input_is_stdin) cupsFileClose(input_file);
    
    LOG_DEBUG_MSG("Running ghostscript. Raster output", filename_bitmap);
#ifdef USE_GHOSTSCRIPT_API
    if (!execute_ghostscript(filename_eps, filename_bitmap, rastermode,
                             lconf.resolution, lconf.height, lconf.width)) {
      LOG_FATAL_STR("ghostscript failed");
      return 1;
    }
#else
    LOG_DEBUG_MSG("                     Vector output", filename_vector);
    if (!execute_ghostscript_cmd(filename_eps, filename_bitmap, filename_vector, rastermode,
                                 lconf.resolution, lconf.height, lconf.width)) {
      LOG_FATAL_STR("ghostscript failed");
      return 1;
    }
#endif
  }

  if (lconf.enable_raster) {
    Raster *raster = Raster::load(filename_bitmap);
    raster->addTile(raster->sourceImage);
    job.addRaster(raster);
  }


  Cut *cut = NULL;
  if (lconf.enable_vector) {
    if (vectorfile) {
      cut = Cut::load(vectorfile);
    }
    else {
#ifdef USE_GHOSTSCRIPT_API
      cut = Cut::load(vectorbuffer);
#else
      cut = Cut::load(filename_vector);
#endif
    }
    job.addCut(cut);
  }

  /* Cleanup unneeded files provided that debug mode is disabled. */
  if (cc_loglevel > CC_DEBUG) {
    if (lconf.enable_raster) {
      if (unlink(filename_bitmap)) {
        LOG_FATAL_MSG("unlink failed", filename_bitmap);
      }
    }

    if (unlink(filename_eps)) {
      LOG_FATAL_MSG("unlink failed", filename_eps);
    }
  }

  Driver drv;
  if (dumpxml) drv.enableXML(true);
  drv.process(&job);

  delete cut;

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
