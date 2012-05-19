#include <stdio.h>
#include <stdlib.h>

#define PJL_PARAM
#include "pjl.hpp"
#include "VectorPass.hpp"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s vector-file\n", argv[0]);
    exit(1);
  }
  const char *filename = argv[1];

  VectorPass *vpass = VectorPass::createFromFile(filename);
  vpass->print(cout);
}
