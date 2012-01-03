#ifndef EPS_CONVERTER_H_
#define EPS_CONVERTER_H_

#include <stdio.h>
#include <cups/file.h>
#include "config/DocumentSettings.hpp"

extern bool ps_to_eps(cups_file_t *ps_file, FILE *eps_file);

#endif
