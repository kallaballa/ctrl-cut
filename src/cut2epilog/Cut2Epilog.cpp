/*
 * Ctrl-Cut - A laser cutter toolchain
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

#include <fstream>
#include "util/Logger.hpp"
#include "Document.hpp"
#include "cut/Cut.hpp"
#include "CtrlCutException.hpp"
#include "svg/CtrlCutParser.hpp"
#include "cutters/EpilogLegend36Ext.hpp"
#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

int main(int argc, char *argv[]) {
  if(argc != 3) {
    std::cerr << "Usage: cut2epilog <cut-file> <output file>" << std::endl;
    exit(1);
  }
  Logger::init(CC_WARNING);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document doc;
  Document::CutList newCuts;
  Document::EngraveList newEngravings;
  string cutFile(argv[1]);
  string epilogFile(argv[2]);

  if(!bfs::exists(cutFile))
    CtrlCutException::fileNotFoundException(cutFile);

  CtrlCutParser parser;
  parser.load(cutFile, doc, newCuts, newEngravings);
  doc.put(DocumentSettings::FILENAME, cutFile);

  for(Cut* cut : newCuts) {
    cut->normalize();
    cut->sort();
    cut->translate();
  }

  std::ofstream oEpilog(epilogFile);
  EpilogLegend36Ext cutter;
  cutter.write(doc,oEpilog);

  oEpilog << std::endl;
  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
