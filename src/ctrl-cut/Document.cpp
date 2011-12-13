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
#include "Document.h"
#include "util/Measurement.h"
using boost::format;
using std::list;

void Document::addCut(CutModel* cut) {
  this->cutList.push_back(cut);
}

void Document::addRaster(Raster* raster) {
  this->engraveList.push_back(raster);
}

typedef EngraveSettings ES;
typedef DocumentSettings DS;
void Document::serializeTo(std::ostream &out) {
  string title = this->settings.get(DS::TITLE);
  int resolution = this->settings.get(DS::RESOLUTION);
  int raster_power = 0;
  int raster_speed = 0;
  if(this->settings.get(DS::ENABLE_RASTER) && !this->engraveList.empty()) {
    raster_power = this->front_engrave()->settings.get(ES::EPOWER);
    raster_speed = this->front_engrave()->settings.get(ES::ESPEED);
  }
  double width = this->settings.get(DocumentSettings::WIDTH).in(PX, resolution);
  double height = this->settings.get(DocumentSettings::HEIGHT).in(PX, resolution);
  bool enable_raster = this->settings.get(DocumentSettings::ENABLE_RASTER);
  bool enable_vector = this->settings.get(DocumentSettings::ENABLE_VECTOR);
  int focus = this->settings.get(DocumentSettings::AUTO_FOCUS);

  /* Print the printer job language header. */
  out << format(PJL_HEADER) % title;
  /* Set autofocus on or off. */
  out << format(PCL_AUTOFOCUS) % focus;
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
  out << format(PCL_PRINT_RESOLUTION) % resolution;
  /* X position = 0 */
  out << format(PCL_POS_X) % 0;
  /* Y position = 0 */
  out << format(PCL_POS_Y) % 0;
  /* PCL resolution. */
  out << format(PCL_RESOLUTION) % resolution;

  // Raster Orientation
  out << format(R_ORIENTATION) % 0;

  // Raster power
  out << format(R_POWER) % raster_power;

  // Raster speed
  out << format(R_SPEED) % raster_speed;

  out << PCL_UNKNOWN_BLAFOO3;
  out << format(R_HEIGHT) % width;
  out << format(R_WIDTH) % height;
  // Raster compression
  int compressionLevel = 2;

  out << format(R_COMPRESSION) % compressionLevel;

  /* If raster power is enabled and raster mode is not 'n' then add that
   * information to the print job.
   */

  if (enable_raster && !this->engraveList.empty()) {
    for (EngraveIt it = this->engraveList.begin(); it != this->engraveList.end(); it++) {
      Raster *raster = *it;
      if (raster) {
        LOG_DEBUG_STR("Encoding raster...");
        PclEncoder r;
        r.encode(out, *raster);
      }
    }
  }

  if (enable_vector && !this->cutList.empty()) {
    // FIXME: This is to emulate the LT bug in the Epilog drivers:
    // Check if any clipping has been done in any of the passes, and
    // inject the stray "LT" string. This has no function, just for bug compatibility
    // of the output files. See corresponding FIXME in LaserJob.cpp.
    for (CutIt it = this->cutList.begin(); it != this->cutList.end(); it++) {
      CutModel *cut = *it;
      if (cut && cut->wasClipped())
        out << "LT";
    }

    out << PCL_SECTION_END;

    /* We're going to perform a vector print. */
    for (CutIt it = this->cutList.begin(); it != this->cutList.end(); it++) {
      CutModel *model = *it;
      HPGLEncoder r;
      r.encode(out,*model);
    }
  }
  out << PCL_SECTION_END << HPGL_PEN_UP;
  out << PCL_RESET;
  out << PJL_FOOTER;

  /* Pad out the remainder of the file with spaces and 'footer'. */
  for (int i = 0; i < 4092; i++) {
    out << ' ';
  }
  out << "Mini]";
}
