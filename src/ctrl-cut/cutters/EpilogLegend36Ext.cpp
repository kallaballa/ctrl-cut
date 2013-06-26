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

#include <vector>
#include "Document.hpp"
#include "EpilogLegend36Ext.hpp"
#include "config/CutSettings.hpp"
#include "util/Measurement.hpp"
#include "encoder/HPGLEncoder.hpp"
#include "encoder/PclEncoder.hpp"
#include "CtrlCutException.hpp"
#include "cut/operations/Translate.hpp"

using boost::format;
using std::list;

typedef EngraveSettings E_SET;
typedef DocumentSettings D_SET;
typedef CutSettings C_SET;

void EpilogLegend36Ext::write(const Document& doc, std::ostream &out) {
  string title = doc.get(D_SET::TITLE);
  int resolution = doc.get(D_SET::RESOLUTION);
  bool enable_engraving = doc.get(D_SET::ENABLE_ENGRAVING) && !doc.engravings().empty();
  bool enable_cut = doc.get(D_SET::ENABLE_CUT) && !doc.cuts().empty();
  int raster_power = 1;
  int raster_speed = 100;
  if(enable_engraving) {
    raster_power = doc.engravings().front()->get(E_SET::EPOWER);
    raster_speed = doc.engravings().front()->get(E_SET::ESPEED);
  }
  double width = doc.get(D_SET::WIDTH).in(PX);
  double height = doc.get(D_SET::HEIGHT).in(PX);
  int focus = doc.get(D_SET::AUTO_FOCUS);

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
  out << format(R_HEIGHT) % height;
  out << format(R_WIDTH) % width;
  // Raster compression
  int compressionLevel = 2;

  out << format(R_COMPRESSION) % compressionLevel;

  /* If raster power is enabled and raster mode is not 'n' then add that
   * information to the print job.
   */

  if (enable_engraving) {
    BOOST_FOREACH(const Engraving* e, doc.engravings()) {
      PclEncoder::encode(out, *e);
    }
  }

  if (enable_cut) {
    // FIXME: This is to emulate the LT bug in the Epilog drivers:
    // Check if any clipping has been done in any of the passes, and
    // inject the stray "LT" string. This has no function, just for bug compatibility
    // of the output files. See corresponding FIXME in LaserJob.cpp.
    /* REFACTOR
    for (CutIt it = this->cutList.begin(); it != this->cutList.end(); it++) {
      Cut *cut = *it;
      if (cut && cut->wasClipped())
        out << "LT";
    }*/

    out << PCL_SECTION_END;

    /* We're going to perform a vector print. */
    BOOST_FOREACH(Cut* cutp, doc.cuts()) {
      Cut& cut = *cutp;
      Cut translated = make_from(cut);
      Point pos = cut.get(CutSettings::CPOS);
      translateTo(cut,translated, pos);
      HPGLEncoder::encode(out,translated);
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
