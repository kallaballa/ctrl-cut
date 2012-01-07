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
//#include "Reduce.hpp"
#include "util/Logger.hpp"
#include "cut/graph/Traverse.hpp"
#include "cut/model/Cut.hpp"
#include <boost/foreach.hpp>
/*!
 * Reduce number of vertices in polylines by approximating the polylines with fewer line segments.
 */

template<typename TmultiPointRange>
void reduce(TmultiPointRange& src, TmultiPointRange& sink, double maxDistance) {
  LOG_INFO_STR("Reduce");
  AddSink<TmultiPointRange> addSink(sink);
  BOOST_FOREACH(const Path& path, src) {
    Path simplyfied(path.settings);
    boost::geometry::simplify(path, simplyfied, maxDistance);
    *addSink++ = simplyfied;
  }
}
