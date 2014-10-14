/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef EPS_CONVERTER_H_
#define EPS_CONVERTER_H_

#include <stdio.h>
#include <cups/file.h>
#include "config/DocumentSettings.hpp"

extern bool ps_to_eps(cups_file_t *ps_file, FILE *eps_file, uint32_t resolution);

#endif
