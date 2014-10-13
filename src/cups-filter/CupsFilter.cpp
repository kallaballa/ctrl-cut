/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "util/Logger.hpp"
#include "Document.hpp"
#include "cut/Cut.hpp"
#include "CupsGetOpt.hpp"
#include <boost/foreach.hpp>
#include "cutters/EpilogLegend36Ext.hpp"

/**
 * Cups filter entry point.
 *
 * @param argc The number of command line options passed to the program.
 * @param argv An array of strings where each string represents a command line
 * argument.
 * @return An integer where 0 represents successful termination, any other
 * value represents an error code.
 */

int main(int argc, char *argv[]) {
  Logger::init(CC_WARNING);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document doc;
  CupsOptions cupsOpts = CupsGetOpt::load_document(doc, argc, argv);

  doc.optimize();

  stringstream ss;

  EpilogLegend36Ext cutter;
  cutter.write(doc,ss);

  std::cout << ss.str() << endl;

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
