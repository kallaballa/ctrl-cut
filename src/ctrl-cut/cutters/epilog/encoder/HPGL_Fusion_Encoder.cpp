/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "cut/Cut.hpp"
#include <boost/format.hpp>
#include <cutters/epilog/encoder/HPGL_Fusion_Encoder.hpp>
#include <cutters/epilog/encoder/PJL.hpp>

using boost::format;

void HPGL_Fusion_Encoder::startPass(std::ostream &out, Cut& encodee) {
  int power_set = encodee.get(CutSettings::CPOWER);
  int speed_set = encodee.get(CutSettings::CSPEED);
  int freq_set = encodee.get(CutSettings::FREQUENCY);

  if(!firstOperation) {
      if (writingPolyline)
        out << SEP;
      out << HPGL_END;
      out << SEP;
    }
    out << HPGL_START;
    out << V_INIT << SEP;
    out << format(V_POWER) % power_set << SEP;
    out << format(V_SPEED) % speed_set << SEP;
    out << format(V_UNKNOWN_FUSION_1) % 0 << SEP;
    out << format(V_UNKNOWN_FUSION_2) % 1 << SEP;
    out << format(V_FREQUENCY) % freq_set; // omit separator;

    firstOperation = false;
}
