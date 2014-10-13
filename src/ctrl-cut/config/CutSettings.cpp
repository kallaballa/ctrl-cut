/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "CutSettings.hpp"

CutSettings::Key<string> CutSettings::CUUID = "cuuid";
CutSettings::Key<CutSettings::Sort> CutSettings::SORT = "optimize";
CutSettings::Key<bool> CutSettings::CLIP = "CLIP";
CutSettings::Key<Point> CutSettings::CPOS = "cpos";
CutSettings::Key<uint16_t> CutSettings::CSPEED = "cspeed";
CutSettings::Key<uint16_t> CutSettings::CPOWER = "cpower";
CutSettings::Key<uint16_t> CutSettings::FREQUENCY = "frequency";
CutSettings::Key<Distance> CutSettings::REDUCE = "reduce";

