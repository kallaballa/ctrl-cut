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

#include "util/Logger.hpp"
#include "Document.hpp"
#include "cut/Cut.hpp"
#include "CupsGetOpt.hpp"
#include "svg/SvgWriter.hpp"
#include "cutters/EpilogLegend36Ext.hpp"

int main(int argc, char *argv[]) {
  typedef DocumentSettings DS;
  Logger::init(CC_WARNING);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document doc;
  CupsOptions cupsOpts = CupsGetOpt::load_document(doc, argc, argv);

  SvgWriter svgW(doc.get(DS::WIDTH).in(PX), doc.get(DS::HEIGHT).in(PX), doc.get(DS::RESOLUTION), doc.get(DS::TITLE), (doc.get(DS::FILENAME) + ".cut").c_str());
  svgW.write(doc, "stroke:rgb(255,0,0);stroke-width:5;");

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
