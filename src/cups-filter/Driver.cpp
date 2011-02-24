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
#include "LaserJob.h"
#include "vector/Cut.h"
#include "vector/filters/Filters.h"

Driver::Driver() : dumpxml(false)
{
}

Driver::~Driver() {
}


/**
 * run the filter stack. additionally if dumpxml is set to truedump cut graph to xml.
 */
void Driver::filter(LaserJob *job) {
  Explode explode;
  Join join;
  Deonion deonion;
  Flat flat;
  Reduce reduce(job->lconf->vector_reduce);
  list<Cut*> cuts = job->getCuts();
  for (list<Cut*>::iterator it = cuts.begin(); it != cuts.end(); it++) {
    Cut *cut = *it;
    if (this->dumpxml) cut->writeXml(job->lconf->datadir + "/" + job->lconf->basename + "-input.xml");
    explode.filter(cut);
    if (this->dumpxml) cut->writeXml(job->lconf->datadir + "/" + job->lconf->basename + "-explode.xml");
    join.filter(cut);
    if (this->dumpxml) cut->writeXml(job->lconf->datadir + "/" + job->lconf->basename + "-join.xml");
    reduce.filter(cut);
    if (this->dumpxml) cut->writeXml(job->lconf->datadir + "/" + job->lconf->basename + "-reduce.xml");
    
    if (job->lconf->vector_optimize == 1) {
      deonion.filter(cut);
      if (this->dumpxml) cut->writeXml(job->lconf->datadir + "/" + job->lconf->basename + "-deonion.xml");
    } else if (job->lconf->vector_optimize == 2) {
      flat.filter(cut);
      if (this->dumpxml) cut->writeXml(job->lconf->datadir + "/" + job->lconf->basename + "-flat.xml");
    }
  }
}

void Driver::process(LaserJob *job) {
  filter(job);
  stringstream ss;
  job->serializeTo(ss);
  LOG_DEBUG_MSG("Output size", ss.str().size());

  std::cout << ss.rdbuf();
}
