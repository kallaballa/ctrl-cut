/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "cutters/epilog/encoder/PJL.hpp"
#include <cutters/epilog/encoder/HPGL_Fusion_Encoder.hpp>
#include <cutters/epilog/Fusion.hpp>
#include <vector>
#include "Document.hpp"
#include "config/CutSettings.hpp"
#include "util/Measurement.hpp"
#include "encoder/PclEncoder.hpp"
#include "CtrlCutException.hpp"
#include "cut/operations/Translate.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"

using boost::format;
using std::list;

typedef EngraveSettings E_SET;
typedef DocumentSettings D_SET;
typedef CutSettings C_SET;

void EpilogFusion::write(const Document& doc, std::ostream &out) {
  string title = doc.get(D_SET::TITLE);
  int resolution = doc.get(D_SET::RESOLUTION);
  bool enableEngraving = doc.get(D_SET::ENABLE_ENGRAVING) && !doc.engravings().empty();
  bool enableCut = doc.get(D_SET::ENABLE_CUT) && !doc.cuts().empty();
  D_SET::Center centerEngrave = doc.get(D_SET::CENTER);
  D_SET::AirAssist airAssist = doc.get(D_SET::AIR_ASSIST);

  int raster_power = 1;
  int raster_speed = 100;
  if(enableEngraving) {
    raster_power = doc.engravings().front()->get(E_SET::EPOWER);
    raster_speed = doc.engravings().front()->get(E_SET::ESPEED);
  }
  double width = doc.get(D_SET::WIDTH).in(PX);
  double height = doc.get(D_SET::HEIGHT).in(PX);
  int focus = doc.get(D_SET::AUTO_FOCUS);

  /* Print the printer job language header. */
  out << format(PJL_HEADER) % title;

  out << format(PCL_UNKNOWN_FUSION_1) % 1536;
  out << format(PCL_DATE) % "20150224165614";
  out << format(PCL_UNKNOWN_FUSION_2) % "130001300003220";
  out << format(PCL_UNKNOWN_FUSION_3) % 0;
  out << format(PCL_UNKNOWN_FUSION_4) % 0;
  out << format(PCL_UNKNOWN_FUSION_5) % 0;

  /* Set autofocus on or off. */
  out << format(PCL_AUTOFOCUS) % focus;
  /* FIXME unknown purpose. */
  out << format(PCL_GLOBAL_AIR_ASSIST) % (airAssist == D_SET::GLOBAL || airAssist == D_SET::CUT_ONLY ? 1 : 0);
  /* enable or disable center engraving. */
  out << format(PCL_CENTER_ENGRAVE) % (centerEngrave != D_SET::CENTER_NONE ? 1 : 0);
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

  out << format(R_UNKNOWN_FUSION_1) % 0;

  // Raster power
  out << format(R_POWER) % raster_power;

  // Raster speed
  out << format(R_SPEED) % raster_speed;

  out << format(R_UNKNOWN_FUSION_2) % 50;

  int rasterAirAssist = 0;
  if(airAssist == D_SET::GLOBAL)
    rasterAirAssist = 2;
  else if(airAssist == D_SET::RASTER_ONLY) {
    rasterAirAssist = 1;
  }

  out << format(PCL_RASTER_AIR_ASSIST) % rasterAirAssist;
  out << format(R_BED_HEIGHT) % height;
  out << format(R_BED_WIDTH) % width;
  // Raster compression
  int compressionLevel = 2;

  out << format(R_COMPRESSION) % compressionLevel;
  //insert a LT right after b%dM like the windows driver does. This actually breaks pcl.
  out << "LT";

  if (enableEngraving) {
    bool first = true;

    for(const EngravingPtr e : doc.engravings()) {
      PclEncoder::encode(out, e);
    }

    if(centerEngrave != D_SET::CENTER_NONE) {
      Box ebox = doc.findEngravingBox();

      //correct the coordinate system
      Point pos = ebox.min_corner;
      Coord_t width = ebox.max_corner.x - ebox.min_corner.x;
      Coord_t height = ebox.max_corner.y - ebox.min_corner.y;
      Coord_t center_x = (width - pos.x) / 2 + pos.x;
      Coord_t center_y = (height - pos.y) / 2 + pos.y;
      switch(centerEngrave) {
      case D_SET::CENTER_CENTER:
        out << format(PCL_OFF_X) % center_x;
        out << format(PCL_OFF_Y) % center_y;
        out << format(PCL_UPPERLEFT_X) % 1;
        out << format(PCL_UPPERLEFT_Y) % 1;
      break;
      case D_SET::CENTER_LEFT:
        out << format(PCL_OFF_X) % 1;
        out << format(PCL_OFF_Y) % center_y;
        out << format(PCL_UPPERLEFT_X) % 1;
        out << format(PCL_UPPERLEFT_Y) % 1;
      break;
      case D_SET::CENTER_RIGHT:
        out << format(PCL_OFF_X) % width;
        out << format(PCL_OFF_Y) % center_y;
        out << format(PCL_UPPERLEFT_X) % 1;
        out << format(PCL_UPPERLEFT_Y) % 1;
      break;
      case D_SET::CENTER_TOP:
        out << format(PCL_OFF_X) % center_x;
        out << format(PCL_OFF_Y) % 1;
        out << format(PCL_UPPERLEFT_X) % 1;
        out << format(PCL_UPPERLEFT_Y) % 1;
      break;
      case D_SET::CENTER_BOTTOM:
        out << format(PCL_OFF_X) % center_x;
        out << format(PCL_OFF_Y) % height;
        out << format(PCL_UPPERLEFT_X) % 1;
        out << format(PCL_UPPERLEFT_Y) % 1;
      break;
      default:
        break;
      }
    }
  }

  if (enableCut) {
    /* We're going to perform a vector print. */
    for(CutPtr cutp : doc.cuts()) {
      Cut& cut = *cutp;
      Cut translated = make_from(cut);
      Point pos = cut.get(CutSettings::CPOS);
      translateTo(cut,translated, pos);
      HPGL_Fusion_Encoder hpgl;
      hpgl.encode(out,translated);
    }
  }
  out << HPGL_START << HPGL_PEN_UP;
  out << PCL_RESET;
  out << PJL_FOOTER;

  /* Pad out the remainder of the file with spaces and 'footer'. */
  for (int i = 0; i < 4092; i++) {
    out << ' ';
  }
  out << "Fusion" << std::endl;
  out.flush();
}
