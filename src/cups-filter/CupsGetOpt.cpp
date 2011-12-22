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

#include "CupsOptions.h"
#include "CupsGetOpt.h"


void CupsGetOpt::print_usage(const string &name) {
  cerr << name << " " << STRINGIFY(CTRLCUT_VERSION) << endl;
  cerr << "Usage: " << name << " [options] job-id user title copies options [file]" << endl << endl;
  cerr << "Options:" << endl;
  cerr << "  -x        Output xml files for debugging" << endl;
  cerr << "  -f <file> Read vectors from the given file instead of the postscript" << endl;
  exit(1);
}

Document& CupsGetOpt::load_document(int argc, char *argv[]) {
  // Extract non-CUPS cmd-line parameters
  typedef DocumentSettings DS;
  Document* doc = new Document();
  // use gs dithering as default for the cups-filter
  doc->settings.put(EngraveSettings::DITHERING, EngraveSettings::DEFAULT_DITHERING);

  int c;
  while ((c = getopt(argc, argv, "x")) != -1) {
    switch (c) {
    case 'x':
      doc->settings.put(DS::DUMP_XML, true);
      break;
    case ':':
      print_usage(argv[0]);
      break;
    case '?':
      print_usage(argv[0]);
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
    print_usage(argv[0]);
  }

  for (int i = optind; i < argc; i++) {
    LOG_DEBUG(argv[i]);
  }

  // FIXME: Register a signal handler to support cancelling of jobs
  string jobid = argv[optind];
  string user = argv[optind + 1];
  string title = argv[optind + 2];
  string copies = argv[optind + 3];
  string options = argv[optind + 4];
  string filename = (cupsargs == 6) ? argv[optind + 5] : NULL;
  doc->settings.put(DS::USER, user);
  doc->settings.put(DS::TITLE, title);

  // Handle CUPS options
  cups_option_t *cups_options;
  size_t numCupsOptions = cupsParseOptions(options.c_str(), 0, &cups_options);
  CupsOptions::parseSettings(doc->settings, cups_options, numCupsOptions);
  doc->load(filename);
  return *doc;
}

void CupsGetOpt::dumpEnv() const {
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
