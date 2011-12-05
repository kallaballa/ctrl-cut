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

#include "Driver.h"
#include <list>
#include "vector/model/CutModel.h"
#include "vector/model/Explode.h"
#include "vector/model/Reduce.h"
#include "vector/graph/Traverse.h"

using std::list;

Driver::Driver() : dumpxml(false)
{
}

Driver::~Driver() {
}

/**
 * run the filter stack. additionally if dumpxml is set to true dump cut graph to xml.
 */
void Driver::filter(LaserJob *job) {
   list<CutModel*> cuts = job->getCuts();
   for (list<CutModel*>::iterator it = cuts.begin(); it != cuts.end(); it++) {
     CutModel& model = *(*it);
     explode_segments(model);
     /* REFACTOR */
     //     reduce_linestrings(model, job->lconf->vector_reduce);
   }
}

void Driver::process(LaserJob *job, std::ostream &stream)
{
  filter(job);
  job->serializeTo(stream);
}
