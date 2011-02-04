/*
 * EpilogCUPS - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PclRenderer.h"

#include "CImg.h"
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "string.h"
#include "stdlib.h"
#include <map>

using std::fstream;
using std::ios;
using std::string;
using std::cerr;
using std::cout;
using namespace cimg_library;

char magic[] = { 0x1b, 0x25, 0x2d, 0x31, 0x32, 0x33, 0x34, 0x35, 0x58, 0x40,
    0x50, 0x4a, 0x4c, 0x20, 0x4a, 0x4f, 0x42, 0x20, 0x4e, 0x41, 0x4d, 0x45,
    0x3d };
char buffer[1024];
int buflen = 1024;
char PCL_DELIM = 0x1b;

PclRenderer::PclRenderer() {
}

PclRenderer::~PclRenderer() {
}

bool readMagic(fstream& rtlfile) {
  if (rtlfile.read(buffer, 23)) {
    return memcmp(magic, buffer, 23) == 0;
  } else
    return false;
}

char* readPclToken(fstream& rtlfile) {
  for (char i = 0; i < (buflen - 1) && rtlfile.good(); i++) {
    buffer[i] = rtlfile.get();
    if (buffer[i] == PCL_DELIM) {
      buffer[i] = '\0';
      return buffer;
    }
  }

  return NULL;
}

PclToken* readPclCommand(fstream& rtlfile) {
  if(!rtlfile.good())
    return NULL;

  const char * signature = "*bW";
  char* token = readPclToken(rtlfile);
  if (token) {
    PclToken* cmd = new PclToken();
    char * endptr;
    char * cursor = token;
    cmd->type = *cursor;
    cmd->prefix = *(++cursor);
    cmd->value = strtol((++cursor), &endptr, 10);

    if (endptr <= cursor)
      return NULL;

    cmd->suffix = *endptr;

    if (memcmp(signature, cmd, 3) == 0) {
      cmd->data = new unsigned char[cmd->value];
      memcpy(cmd->data, (++endptr), cmd->value);
    }

    return cmd;
  }

  return NULL;
}

int mains(int argc, char *argv[]) {
  fstream rtlfile("rgb-stripes.prn", ios::in
      | ios::binary);
  if (!readMagic(rtlfile)) {
    cerr << "Couldn't read magic" << std::endl;
  }

  char * title;
  if (!(title = readPclToken(rtlfile))) {
    cerr << "Couldn't read title" << std::endl;
  }

  if (!readPclToken(rtlfile)) {
    cerr << "Couldn't skip pcl intro" << std::endl;
  }

  PclToken* cmd;
  const char * endHeader = "*rA";
  std::map<string, PclToken*> parameters;
  while ((cmd = readPclCommand(rtlfile)) && !(memcmp(cmd, endHeader, 3) == 0)) {
    parameters[(char*) cmd] = cmd;
  }

  std::map<string, PclToken*>::iterator it;
  for (it = parameters.begin(); it != parameters.end(); it++) {
    cerr << "param: " << (*it).first << std::endl;
  }

  PclToken* height = parameters["*rT"];
  PclToken* width = parameters["*rS"];
  CImg<unsigned char> img(width->value, height->value, 1, 1, 255);

  PclToken *xCmd, *yCmd, *pixlen, *bytelen;
  cerr << "loop" << std::endl;
  unsigned char rl;
  unsigned char intensity;
  int off;
  int x;

  do {
    yCmd = readPclCommand(rtlfile);
    if(!(yCmd && memcmp(yCmd, "*pY", 3) == 0))
      continue;
    xCmd = readPclCommand(rtlfile);
    pixlen = readPclCommand(rtlfile);
    bytelen = readPclCommand(rtlfile);
    off = 0;

    for (int i = 0; bytelen && bytelen->data && (i < bytelen->value); ++i) {
      rl = *bytelen->data++;


      if (rl > 128) {
        rl = 257 - rl;
        cerr << "rl: " << (int) rl << std::endl;
        intensity = 255 - *bytelen->data++;

        for (int j = 0; j < rl; ++j) {
          if(pixlen->value > 0) {
            x = off + xCmd->value + j;
          } else {
            x = xCmd->value - off + (pixlen->value * -1) - j - 1;
          }
          img(x, yCmd->value, 0) = intensity;
          cerr << x << "/" << yCmd->value << "=" << (unsigned int) intensity << "\t";
        }
      } else {
        cerr << "rl: " << (int) rl << std::endl;
        for (int j = 0; j < rl; ++j) {
          intensity = *bytelen->data++;
      //    img(off + xCmd->value + j, yCmd->value, 0) = intensity;

          cerr << (xCmd->value + j) << "/" << yCmd->value << "="
              << (unsigned int) intensity << "\t";
        }
      }
      off += rl;
      cerr << std::endl;
    }
  } while (rtlfile.good());

  //cerr << "after" << std::endl;
  img.crop(0, height->value -100, width->value, height->value, 1);
  img.save_png("/tmp/rtlrender.png", 1);
  return 0;
}
