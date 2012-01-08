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

#include "util/Logger.hpp"
#include "cut/model/Cut.hpp"
#include "CupsGetOpt.hpp"
#include "Document.hpp"

#include "cut/model/Clip.hpp"
#include "cut/model/Reduce.hpp"
#include "cut/model/Explode.hpp"
#include "cut/geom/sink/AddSink.hpp"
#include "cut/graph/Traveller.hpp"
#include "cut/graph/Traverse.hpp"
#include "cut/graph/Planar.hpp"
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

  Coord_t dpi = doc.get(DocumentSettings::RESOLUTION);
  Coord_t width = doc.get(DocumentSettings::WIDTH).in(PX, dpi);
  Coord_t height = doc.get(DocumentSettings::HEIGHT).in(PX, dpi);
  string v;
  Measurement reduceMax(0.1,MM);
  if(cupsOpts.get(CupsOptions::VECTOR_REDUCE, v)) {
    reduceMax = Measurement(boost::lexical_cast<uint16_t>(v), MM);
  }

  for (Document::CutIt it = doc.begin_cut(); it != doc.end_cut(); it++) {
    CutModel& model = **it;
    dump("input.txt", model.begin(), model.end());

    CutModel clipped(model.settings);
    clip(MultiSegmentView<CutModel>(model), AddSink<CutModel>(clipped), Box(Point(0,0),Point(width,height)));
    dump("clipped.txt", clipped.begin(), clipped.end());

    CutModel exploded(model.settings);
    explode(MultiSegmentView<CutModel>(clipped), AddSink<CutModel>(exploded));
    dump("exploded.txt", exploded.begin(), exploded.end());

    CutModel planared(model.settings);
    makePlanar(exploded, planared);
    dump("planared.txt", planared.begin(), planared.end());

    CutModel reduced(model.settings);
    reduce(exploded, reduced, reduceMax.in(PX, dpi));
    dump("reduced.txt", reduced.begin(), reduced.end());
/*
    CutModel travelled = model.make();
    travel(reduced, travelled);
    dump("travelled.txt", travelled.begin(), travelled.end());

    model = travelled;*/
    model = reduced;
    dump("after-copy.txt", model.begin(), model.end());
  }

  std::stringstream ss;
  doc.write(ss);

  LOG_DEBUG_MSG("Output size", ss.str().size());
  std::cout << ss.rdbuf();

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
