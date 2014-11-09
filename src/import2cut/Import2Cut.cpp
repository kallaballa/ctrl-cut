/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "util/Logger.hpp"
#include "Document.hpp"
#include "svg/SvgWriter.hpp"
#include <boost/program_options.hpp>
#include "config/EngraveSettings.hpp"
#include <fstream>
#include <string>

using std::string;

int main(int argc, char *argv[]) {
  namespace po = boost::program_options;

  Logger::init(CC_WARNING);
  clock_t start = clock();
  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  po::options_description genericDesc("Options");
  size_t resolution = 75;
  string inputFile;
  string outputFile;
  genericDesc.add_options()
      ("resolution,r", po::value<size_t>(&resolution), "The document resolution")
      ("help,h", "Produce help message");

  po::options_description hidden("Hidden options");
  hidden.add_options()("input-file", po::value<string>(&inputFile), "The input file");
  hidden.add_options()("output-file", po::value<string>(&outputFile), "The output file");

  po::options_description cmdline_options;
  cmdline_options.add(genericDesc).add(hidden);

  po::positional_options_description p;
  p.add("input-file", 1);
  p.add("output-file", 1);

  po::options_description visible;
  visible.add(genericDesc);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
  po::notify(vm);


  if (vm.count("help")) {
    std::cerr << "Usage: import2cut [options] <input-file> <output-file>" << std::endl;
    std::cerr << visible;
    return 0;
  }

  Document doc;
  doc.put(DocumentSettings::RESOLUTION, resolution);
  doc.put(DocumentSettings::ENABLE_ENGRAVING, true);
  doc.put(DocumentSettings::LOAD_ENGRAVING, true);
  doc.put(EngraveSettings::DITHERING, EngraveSettings::FLOYD_STEINBERG);


  doc.load(inputFile);
  
  std::ofstream os(outputFile);
  SvgWriter svgW(doc, os);
  svgW.write(doc, "stroke:rgb(255,0,0);stroke-width:5;");

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);
  return 0;
}
