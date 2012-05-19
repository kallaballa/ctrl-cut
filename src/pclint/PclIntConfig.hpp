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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "2D.hpp"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

enum DEBUG_LEVEL {
  LVL_QUIET, LVL_INFO, LVL_WARN, LVL_DEBUG
};
class PclIntConfig {
private:
  PclIntConfig(): interactive(false), autocrop(false), clip(NULL), screenSize(NULL), ifilename(NULL), rasterFilename(NULL), vectorFilename(NULL), combinedFilename(NULL), debugLevel(LVL_WARN) {};
  static PclIntConfig* instance;
public:
  bool interactive;
  bool autocrop;
  BoundingBox *clip;
  BoundingBox *screenSize;
  char *ifilename;
  char *rasterFilename;
  char *vectorFilename;
  char *combinedFilename;
  DEBUG_LEVEL debugLevel;

  static PclIntConfig* singleton();

  void parseCommandLine(int argc, char *argv[]) {
    int c;
    opterr = 0;
    while (optind < argc) {
      while ((c = getopt(argc, argv, "iac:r:v:d:s:")) != -1) {
        switch (c) {
        case 'i':
          this->interactive = true;
          break;
        case 'a':
          this->autocrop = true;
          break;
        case 'c':
          this->clip = BoundingBox::createFromGeometryString(optarg);
          break;
        case 'r':
          this->rasterFilename = optarg;
          break;
        case 'v':
          this->vectorFilename = optarg;
          break;
        case 'b':
          this->combinedFilename = optarg;
          break;
        case 'd':
          if(strcmp("quiet", optarg) == 0)
            debugLevel = LVL_QUIET;
          else if(strcmp("info", optarg) == 0)
            debugLevel = LVL_INFO;
          else if(strcmp("warn", optarg) == 0)
            debugLevel = LVL_WARN;
          else if(strcmp("debug", optarg) == 0)
            debugLevel = LVL_DEBUG;
          else
            printUsage();
          break;
        case 's':
          this->screenSize = BoundingBox::createFromGeometryString(optarg);
          break;
        case ':':
          printUsage();
          break;
        case '?':
          printUsage();
          break;
        }
      }
      if (optind < argc) {
        if (!this->ifilename) {
          this->ifilename = argv[optind++];
        }
        else {
          printUsage();
        }
      }
    }

    // Required parameters
    if (!this->ifilename) {
      printUsage();
    }
  }

  void printUsage()
  {
    fprintf(stderr, "pclint %s\n", TOSTRING(CTRLCUT_VERSION));
    fprintf(stderr,
        "Usage: pclint [options] <PCL file> [<output file>]\n\n");

    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -i                Enter interactive mode\n");
    fprintf(stderr, "  -a                Automatically crop the output image to the detected bounding box\n");
    fprintf(stderr, "  -c <bbox>         Clip to given bounding box\n");
//    fprintf(stderr, "  -b <filename>     Output the combined job to the given filename\n");
    fprintf(stderr, "  -v <filename>     Output the vector pass to the given filename\n");
    fprintf(stderr, "  -r <filename>     Output the raster pass to the given filename\n");
    fprintf(stderr, "  -d <level>        Set the verbosity level (quiet/info/warn/debug)\n");
    fprintf(stderr, "  -s <dimension>    Configure the size of the live rendering window. e.g. 1024x768\n");
    exit(1);
  }
};

PclIntConfig* PclIntConfig::instance = NULL;
PclIntConfig* PclIntConfig::singleton() {
  if (instance == NULL)
    instance = new PclIntConfig();

  return instance;
}

#endif /* CONFIG_H_ */
