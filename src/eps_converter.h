#ifndef EPS_CONVERTER_H_
#define EPS_CONVERTER_H_

#include <stdio.h>

extern char buf[102400];
extern bool ps_to_eps(class laser_config *lconf, FILE *ps_file, FILE *eps_file);

#endif
