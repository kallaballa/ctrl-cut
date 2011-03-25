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

#include "2D.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class PclIntConfig {
private:
  PclIntConfig(): interactive(false), autocrop(false), clip(NULL), ifilename(NULL), rasterFilename(NULL), vectorFilename(NULL) {};
  static PclIntConfig* instance;
public:
  bool interactive;
  bool autocrop;
  BoundingBox *clip;
  char* ifilename;
  char* rasterFilename;
  char* vectorFilename;

  static PclIntConfig* singleton();

  void parseCommandLine(int argc, char *argv[]) {
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "iac:r:v:")) != -1)
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
      case ':':
        printUsage();
        break;
      case '?':
        printUsage();
        break;
      }

    if (optind < argc) {
      this->ifilename = argv[optind];
    } else {
      printUsage();
    }
  }

  void printUsage()
  {
    fprintf(stderr, "pclint %s\n", TOSTRING(CTRLCUT_VERSION));
    fprintf(stderr,
        "Usage: pclint [options] <PCL file> [<output file>]\n\n");

    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -i        Enter interactive mode\n");
    fprintf(stderr, "  -a        Automatically crop the output image to the detected bounding box\n");
    fprintf(stderr, "  -c <bbox> Clip to given bounding box\n");
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
