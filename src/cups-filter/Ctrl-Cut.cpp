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
#include "config/LaserConfig.h"
#include "config/CtrlCutOptions.h"
#include "util/Eps.h"
#include "vector/model/CutModel.h"
#include "LaserJob.h"
#include "Driver.h"
#include "Ctrl-Cut.h"

#include <cups/cups.h>
#include <cups/file.h>

#include "FileParser.h"
#include <boost/thread.hpp>
#include "svg/Svg2Ps.h"

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

  CtrlCutOptions options;
  options.parseGetOpt(argc, argv);

  // Perform a check over the global values to ensure that they have values
  // that are within a tolerated range.
  LaserConfig::getInstance().rangeCheck();

  LaserJob job = options.createJob();

  string filename_vector;
  string filename_pbm;

  enum { FORMAT_POSTSCRIPT, FORMAT_VECTOR, FORMAT_PBM } inputformat = FORMAT_POSTSCRIPT;

  cups_file_t *input_file;

  if (options.filename.empty()) {
    input_file = cupsFileStdin();

    LaserConfig::getInstance().datadir = "/tmp";
    LaserConfig::getInstance().basename = "stdin";
  } else {
    LaserConfig::getInstance().datadir = dirname(strdup(options.filename.c_str()));
    string base = basename(strdup(options.filename.c_str()));
    
    string suffix = base.substr(base.rfind(".") + 1);
    transform ( suffix.begin(), suffix.end(), suffix.begin(), lower_case );

    LaserConfig::getInstance().basename = base.erase(base.rfind("."));
    
    if (suffix == "vector") {
      inputformat = FORMAT_VECTOR;
      filename_vector = options.filename;
    } else if (suffix == "pbm") {
      inputformat = FORMAT_PBM;
      filename_pbm = options.filename;
    } else if (suffix == "svg") {
      // Try to open the print file...
      int convertPipe[2];
      FILE *svgIn = fopen(options.filename.c_str(), "r");
      int svgFd = fileno (svgIn);

      if (pipe(convertPipe)) {
        fprintf(stderr, "Pipe failed.\n");
        return EXIT_FAILURE;
      }

      Svg2Ps converter(svgFd, convertPipe[1]);
      boost::thread svg_converter_thread(&Svg2Ps::convert, converter);

      if ((input_file = cupsFileOpenFd(convertPipe[0], "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", options.filename.c_str());
        return 1;
      }
    } else {
      // Try to open the print file...
      if ((input_file = cupsFileOpen(options.filename.c_str(), "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", options.filename.c_str());
        return 1;
      }
    }
  }

  FileParser *parser = NULL;

  if (inputformat == FORMAT_POSTSCRIPT) {
    string file_basename = LaserConfig::getInstance().tempdir + "/" + LaserConfig::getInstance().basename.c_str();
    
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

    PostscriptParser *psparser = new PostscriptParser(LaserConfig::getInstance());
    // Uncomment this to force ghostscript to render to file using the ppmraw
    // backend, instead of in-memory rendering
    //    psparser->setRenderToFile(true);
    switch (LaserConfig::getInstance().raster_dithering) {
    case LaserConfig::DITHER_DEFAULT:
      psparser->setRasterFormat(PostscriptParser::BITMAP);
      break;
    case LaserConfig::DITHER_BAYER:
    case LaserConfig::DITHER_FLOYD_STEINBERG:
    case LaserConfig::DITHER_JARVIS:
    case LaserConfig::DITHER_BURKE:
    case LaserConfig::DITHER_STUCKI:
    case LaserConfig::DITHER_SIERRA2:
    case LaserConfig::DITHER_SIERRA3:
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

  if (LaserConfig::getInstance().enable_raster) {
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
      raster->addTile(raster->sourceImage());
      job.addRaster(raster);
    }
  }

  CutModel *cut = NULL;
  if (LaserConfig::getInstance().enable_vector) {
    if (inputformat == FORMAT_VECTOR) {
      cut = CutModel::load(LaserConfig::getInstance(),filename_vector);
    }
    else if (parser) {
      cut = CutModel::load(LaserConfig::getInstance(),parser->getVectorData());
    }
    if (cut) job.addCut(cut);
  }

  Driver drv;
  if (options.dumpXML) drv.enableXML(true);
  std::stringstream ss;
  drv.process(&job, ss);
  LOG_DEBUG_MSG("Output size", ss.str().size());
  std::cout << ss.rdbuf();

  delete cut;
  delete parser;

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
