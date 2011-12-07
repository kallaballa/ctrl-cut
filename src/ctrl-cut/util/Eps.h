#ifndef EPS_CONVERTER_H_
#define EPS_CONVERTER_H_

#include <stdio.h>
#include <cups/file.h>
#include "config/DocumentSettings.h"

extern char buf[102400];
extern bool ps_to_eps(DocumentSettings *lconf, cups_file_t *ps_file, FILE *eps_file);

#endif
