#ifndef EPS_CONVERTER_H_
#define EPS_CONVERTER_H_

#include <stdio.h>
#include "LaserConfig.h"
#include <cups/file.h>

extern char buf[102400];
extern bool ps_to_eps(LaserConfig *lconf, cups_file_t *ps_file, FILE *eps_file);

#endif
