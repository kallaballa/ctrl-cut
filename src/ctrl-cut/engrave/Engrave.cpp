/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "Engrave.hpp"

Box find_bounding_box(Engraving& eng) {
  Point pos = eng.get(EngraveSettings::EPOS);
  return Box(pos, Point(pos.x + eng.getImage().width(), pos.y + eng.getImage().height()));
}
