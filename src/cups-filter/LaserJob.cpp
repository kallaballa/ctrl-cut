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
#define PJL_PARAM
#include <list>
#include "LaserJob.h"
#include "vector/model/CutModel.h"

using boost::format;
using std::list;

LaserJob::LaserJob(LaserConfig *lconf, const string &user, const string &name,
    const string &title) {
  this->lconf = lconf;
  this->user = user;
  this->name = name;
  this->title = title;
}

LaserJob::~LaserJob() {
  // TODO Auto-generated destructor stub
}

void LaserJob::addCut(CutModel* cut) {
  this->cuts.push_back(cut);
}

void LaserJob::addRaster(Raster* raster) {
  this->rasters.push_back(raster);
}

// FIXME: LAOS hack
void LaserJob::serializeTo(std::ostream &out)
{
  /* We're going to perform a vector print. */
  for (list<CutModel*>::iterator it = this->cuts.begin(); it != this->cuts.end(); it++) {
    CutModel *model = *it;
    this->vectorencoder->encode(*model, out);
  }
}

#if 0
/**
 *
 */
void LaserJob::serializeTo(std::ostream &out) {
  int i;
  /* Print the printer job language header. */

  out << format(PJL_HEADER) % this->title.c_str();
  /* Set autofocus on or off. */
  out << format(PCL_AUTOFOCUS) % this->lconf->focus;
  /* FIXME unknown purpose. */
  out << PCL_UNKNOWN_BLAFOO;
  /* FIXME unknown purpose. */
  out << PCL_UNKNOWN_BLAFOO2;
  /* Left (long-edge) offset registration.  Adjusts the position of the
   * logical page across the width of the page.
   */
  out << format(PCL_OFF_X) % 0;
  /* Top (short-edge) offset registration.  Adjusts the position of the
   * logical page across the length of the page.
   */
  out << format(PCL_OFF_Y) % 0;
  /* Resolution of the print. */
  out << format(PCL_PRINT_RESOLUTION) % this->lconf->resolution;
  /* X position = 0 */
  out << format(PCL_POS_X) % 0;
  /* Y position = 0 */
  out << format(PCL_POS_Y) % 0;
  /* PCL resolution. */
  out << format(PCL_RESOLUTION) % this->lconf->resolution;

  // Raster Orientation
  out << format(R_ORIENTATION) % 0;

  // Raster power
  out << format(R_POWER) % lconf->raster_power;

  // Raster speed
  out << format(R_SPEED) % lconf->raster_speed;

  out << PCL_UNKNOWN_BLAFOO3;
  out << format(R_HEIGHT) % ((lconf->height * lconf->resolution)
      / POINTS_PER_INCH);
  out << format(R_WIDTH) % ((lconf->width * lconf->resolution)
      / POINTS_PER_INCH);
  // Raster compression
  int compressionLevel = 2;

  out << format(R_COMPRESSION) % compressionLevel;

  /* If raster power is enabled and raster mode is not 'n' then add that
   * information to the print job.
   */

  if (this->lconf->enable_raster && !this->rasters.empty()) {
    for (list<Raster*>::iterator it = this->rasters.begin(); it != this->rasters.end(); it++) {
      Raster *raster = *it;
      if (raster) {
        LOG_DEBUG_STR("Encoding raster...");
        PclEncoder r(this->lconf);
        r.encode(raster, out);
      }
    }
  }

  if (this->lconf->enable_vector && !this->cuts.empty()) {
    // FIXME: This is to emulate the LT bug in the Epilog drivers:
    // Check if any clipping has been done in any of the passes, and
    // inject the stray "LT" string. This has no function, just for bug compatibility
    // of the output files. See corresponding FIXME in LaserJob.cpp.
    for (list<CutModel*>::iterator it = this->cuts.begin(); it != this->cuts.end(); it++) {
      CutModel *cut = *it;
      if (cut && cut->wasClipped())
        out << "LT";
    }

    out << PCL_SECTION_END;

    /* We're going to perform a vector print. */
    for (list<CutModel*>::iterator it = this->cuts.begin(); it != this->cuts.end(); it++) {
      CutModel *model = *it;
      LaosEncoder r(this->lconf);
      r.encode(*model,out);
    }
  }
  out << PCL_SECTION_END << HPGL_PEN_UP;
  out << PCL_RESET;
  out << PJL_FOOTER;

  /* Pad out the remainder of the file with spaces and 'footer'. */
  for (i = 0; i < 4092; i++) {
    out << ' ';
  }
  out << "Mini]";
}
#endif
