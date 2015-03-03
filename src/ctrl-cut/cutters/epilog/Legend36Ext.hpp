/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#define PJL_PARAM

#include "Document.hpp"
#include <ostream>

class EpilogLegend36Ext {
public:
  void write(const Document& doc, std::ostream &out);
};
