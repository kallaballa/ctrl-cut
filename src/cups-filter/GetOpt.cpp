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

#include "GetOpt.h"
#include "CupsOptions.h"

void GetOpt::printUsage(const string &name) {
  cerr << name << " " << STRINGIFY(CTRLCUT_VERSION) << endl;
  cerr << "Usage: " << name << " [options] job-id user title copies options [file]" << endl << endl;
  cerr << "Options:" << endl;
  cerr << "  -x        Output xml files for debugging" << endl;
  cerr << "  -f <file> Read vectors from the given file instead of the postscript" << endl;
  exit(1);
}

void GetOpt::parseDocumentSettings(DocumentSettings& docSettings, int argc, char *argv[]) {
  typedef DocumentSettings DS;
  // Extract non-CUPS cmd-line parameters
  int c;
  while ((c = getopt(argc, argv, "x")) != -1) {
    switch (c) {
    case 'x':
      this->dumpXML = true;
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

  // FIXME: Register a signal handler to support cancelling of jobs
  this->jobid = argv[optind];
  this->user = argv[optind + 1];
  this->title = argv[optind + 2];
  this->copies = argv[optind + 3];
  this->options = argv[optind + 4];
  this->filename = (cupsargs == 6) ? argv[optind + 5] : NULL;
  docSettings.put(DS::USER, user);
  docSettings.put(DS::TITLE, title);

  // Handle CUPS options
  cups_option_t *cups_options;
  size_t numCupsOptions = cupsParseOptions(options.c_str(), 0, &cups_options);
  CupsOptions cupsOptions(cups_options, numCupsOptions);
  cupsOptions.parseSettings(docSettings);
}

void GetOpt::dumpEnv() const {
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
}
