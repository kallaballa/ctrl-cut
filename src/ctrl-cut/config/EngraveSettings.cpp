/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "EngraveSettings.hpp"

EngraveSettings::Key<string> EngraveSettings::EUUID = "euuid";
EngraveSettings::Key<EngraveSettings::Dithering> EngraveSettings::DITHERING = "dithering";
EngraveSettings::Key<EngraveSettings::Direction> EngraveSettings::DIRECTION = "direction";
EngraveSettings::Key<Point> EngraveSettings::EPOS = "epos";
EngraveSettings::Key<uint16_t> EngraveSettings::EPOWER = "epower";
EngraveSettings::Key<uint16_t> EngraveSettings::ESPEED = "espeed";

