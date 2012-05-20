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
#include "cut/model/Explode.hpp"
#include "cut/model/Reduce.hpp"
#include "cut/geom/sink/AddSink.hpp"

#include "cut/graph/Planar.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

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
  typedef DocumentSettings DS;
  Logger::init(CC_WARNING);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document doc;
  CupsOptions cupsOpts = CupsGetOpt::load_document(doc, argc, argv);

  Coord_t dpi = doc.get(DS::RESOLUTION);
  Coord_t width = doc.get(DS::WIDTH).in(PX);
  Coord_t height = doc.get(DS::HEIGHT).in(PX);
  string v;
  Measurement reduceMax(0.2,MM, dpi);
  if(cupsOpts.get(CupsOptions::VECTOR_REDUCE, v)) {
    reduceMax = Measurement(boost::lexical_cast<uint16_t>(v), MM, dpi);
  }
  reduceMax = Measurement(0.1, MM, dpi);
  BOOST_FOREACH(Cut* p_cut, doc.cutList) {
    Cut& cut = *p_cut;
    dump("input.txt", cut.begin(), cut.end());

    Cut clipped = make_from(cut);
    Cut planar = make_from(cut);
    Cut exploded = make_from(cut);
    Cut reduced = make_from(cut);

    clip(cut, clipped, Box(Point(0,0),Point(width,height)));
    dump("clipped.txt", clipped.begin(), clipped.end());
    clipped.check();
    cut.clear();

    explode(clipped, exploded);
    dump("exploded.txt", exploded.begin(), exploded.end());
    exploded.check();

    make_planar(exploded, planar);
    dump("planar.txt", planar.begin(), planar.end());
    planar.check();

    reduce(planar, reduced, reduceMax.in(PX));
    dump("reduced.txt", reduced.begin(), reduced.end());
    reduced.check();

    cut = reduced;
    cut.check();
    dump("cut.txt", cut.begin(), cut.end());
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
