/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#include "CupsGetOpt.hpp"

void CupsGetOpt::print_usage(const string &name) {
  cerr << name << " " << STRINGIFY(CTRLCUT_VERSION) << endl;
  cerr << "Usage: " << name << " [options] job-id user title copies options [file]" << endl << endl;
  cerr << "Options:" << endl;
  cerr << "  -x        Output xml files for debugging" << endl;
  cerr << "  -f <file> Read vectors from the given file instead of the postscript" << endl;
  exit(1);
}

CupsOptions CupsGetOpt::load_document(Document& doc, int argc, char *argv[]) {
  // Extract non-CUPS cmd-line parameters
  typedef DocumentSettings DS;
  // use gs dithering as default for the cups-filter
  doc.put(EngraveSettings::DITHERING, EngraveSettings::FLOYD_STEINBERG);
  doc.put(DocumentSettings::LOAD_ENGRAVING, true);
  int c;
  while ((c = getopt(argc, argv, "x")) != -1) {
    switch (c) {
    case 'x':
      doc.put(DS::DUMP_XML, true);
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
  doc.put(DS::USER, user);
  doc.put(DS::TITLE, title);

  // Handle CUPS options
  cups_option_t *cups_options;
  size_t numCupsOptions = cupsParseOptions(options.c_str(), 0, &cups_options);
  CupsOptions cupsOpts = CupsOptions::parseSettings(doc.settings(), cups_options, numCupsOptions);
  doc.load(filename);
  return cupsOpts;
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
