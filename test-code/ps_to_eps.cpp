#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "laser_config.h"
#include "eps_converter.h"

/** Temporary buffer, used to eps_converter */
char buf[102400];

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <ps-file>\n", argv[0]);
    exit(1);
  }

  laser_config lconf;
  init_laser_config(&lconf);

  FILE *file_ps = fopen(argv[1], "r");
  if (!file_ps) {
    fprintf(stderr, "Unable to open input file '%s'\n", argv[1]);
    exit(1);
  }

  if (!ps_to_eps(&lconf, file_ps, stdout)) {
    fprintf(stderr, "Error: ps_to_eps() failed\n");
  }

  fclose(file_ps);
}
