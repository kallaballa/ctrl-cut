/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef PPMFILE_H_
#define PPMFILE_H_

#define PPMREADBUFLEN 256

#include "stdio.h"
#include <fstream>
#include "util/Logger.hpp"
#include "MMapMatrix.hpp"

static inline GrayscaleImage loadppm(std::string filename)
{
  LOG_DEBUG_MSG("loading PPM..", filename);

  char buf[PPMREADBUFLEN], *t;
  unsigned int w, h, d;
  int r;
  int data_offset = 0;
  FILE* pf = fopen(filename.c_str(), "r");

  assert (pf != NULL);
  t = fgets(buf, PPMREADBUFLEN, pf);
  assert (t != NULL);

  uint8_t components;
  if (!strncmp(buf, "P6\n", 3)) components = 3;
  else if (!strncmp(buf, "P5\n", 3)) components = 1;
  else assert(false);

  data_offset += 4;

  do { /* Px formats can have # comments after first line */
    t = fgets(buf, PPMREADBUFLEN, pf);
    assert (t != NULL);
    data_offset += strlen(buf);
  } while (strncmp(buf, "#", 1) == 0);
  r = sscanf(buf, "%u %u", &w, &h);

  assert(r >= 2);
  // The program fails if the first byte of the image is equal to 32. because
  // the fscanf eats the space and the image is read with some bit less
  r = fscanf(pf, "%u\n", &d);
  data_offset += 3;
  assert ((r >= 1) && (d == 255));
  fpos_t pos;

  fgetpos(pf, &pos);
  return MappedImage(filename, w, h, components, data_offset);
}

static inline BitmapImage loadpbm(std::string filename)
{
  LOG_DEBUG_MSG("loading PBM..", filename);

  char buf[PPMREADBUFLEN], *t;
  unsigned int w, h;
  int r;
  int data_offset = 0;
  FILE* pf = fopen(filename.c_str(), "r");

  assert (pf != NULL);
  t = fgets(buf, PPMREADBUFLEN, pf);
  assert (t != NULL);

  assert (!strncmp(buf, "P4\n", 3));

  data_offset += 4;

  do { /* Px formats can have # comments after first line */
    t = fgets(buf, PPMREADBUFLEN, pf);
    assert (t != NULL);
    data_offset += strlen(buf);
  } while (strncmp(buf, "#", 1) == 0);
  r = sscanf(buf, "%u %u", &w, &h);

  assert (r >= 2);
  fpos_t pos;
  fgetpos(pf, &pos);

  size_t bufsize = w*h/8;
  uint8_t *imgbuf = new uint8_t[bufsize];
  size_t items = fread(imgbuf, bufsize, 1, pf);
  if (items == 1) {
    for (uint32_t i=0;i<bufsize;i++) imgbuf[i] = ~imgbuf[i];
    BitmapImage img(w,h,imgbuf);
    return img;
  }
  else {
    delete[] imgbuf;
    assert(false);
  }
}

#endif /* PPMFILE_H_ */
