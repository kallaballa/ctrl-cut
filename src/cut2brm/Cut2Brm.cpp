/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include <fstream>
#include <iostream>
#include "util/Logger.hpp"
#include "Document.hpp"
#include "cut/Cut.hpp"
#include "CtrlCutException.hpp"
#include "svg/CtrlCutParser.hpp"
#include "cutters/Brm90130.hpp"
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

int main(int argc, char *argv[]) {
  if(argc != 3) {
    std::cerr << "Usage: cut2brm <cut-file> <output file>" << std::endl;
    exit(1);
  }
  Logger::init(CC_DEBUG);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document doc;
  Document::CutList newCuts;
  Document::EngraveList newEngravings;
  string cutFile(argv[1]);
  string brmFile(argv[2]);

  if(!bfs::exists(cutFile))
    CtrlCutException::fileNotFoundException(cutFile);

  CtrlCutParser parser;
  parser.load(cutFile, doc, newCuts, newEngravings);
  doc.put(DocumentSettings::FILENAME, cutFile);

  doc.mergeCuts();
  doc.optimize();

  std::ofstream oBrm(brmFile, std::ios::out | std::ios::binary);
  Brm90130 cutter;
  cutter.write(doc, oBrm);

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
