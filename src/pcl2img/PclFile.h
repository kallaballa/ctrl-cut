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

#ifndef PCLFILE_H_
#define PCLFILE_H_

#include "stdint.h"
#include "climits"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "CImg.h"

using std::cerr;
using std::endl;
using std::string;
using std::fstream;
using std::ios;
using std::map;
using namespace cimg_library;

#define PCL_X "*pX"
#define PCL_Y "*pY"
#define PCL_WIDTH "*rS"
#define PCL_HEIGHT "*rT"
#define PCL_RLE "*bW"
#define PCL_PIXEL_LEN "*bA"
#define PCL_END_OF_SETTINGS "*rA"
#define PCL_END_OF_INSTRUCTION 0x1b

char magic[] = { 0x1b, 0x25, 0x2d, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x58, 0x40, 0x50, 0x4a, 0x4c, 0x20, 0x4a, 0x4f, 0x42, 0x20, 0x4e,
        0x41, 0x4d, 0x45, 0x3d };

class PclInstruction {
public:
  PclInstruction() : keysep('\0'), pos(0), limit(UCHAR_MAX) {}
  char type;
  char prefix;
  char suffix;
  char keysep; // separate the instr signature from value/data so it may be interpreted as a string and used as key
  int32_t value;
  uint8_t * data;
  uint8_t pos;
  uint8_t limit;

  bool hasNext(bool report=true) {
    bool hasnext = this->pos < this->limit;
    if(!hasnext && report)
        std::cerr << "short read" << std::endl;
    return hasnext;
  }

  uint8_t next(){
      return (uint8_t) *(this->data+this->pos++);
  }

  bool matches(const string& signature) {
    return memcmp(this, signature.c_str(), 3) == 0;
  }
};

class PclFile {
private:
  fstream pclfile;
  const char* title;
  const int buflen;
  char* buffer;
  bool valid;

  bool readPattern(const char * pattern, const int off, const int len) {
    if (this->pclfile.read(buffer, len)) {
      return memcmp((pattern + off), buffer, len) == 0;
    } else
      return false;
  }

  char* readInstrBuffer() {
    for (int i = 0; i < (buflen - 1) && this->pclfile.good(); i++) {
      buffer[i] = this->pclfile.get();
      if (buffer[i] == PCL_END_OF_INSTRUCTION) {
        buffer[i] = '\0';
        return buffer;
      }
    }

    return NULL;
  }

public:
  uint32_t width;
  uint32_t height;
  std::map<string, PclInstruction*> settings;

  PclFile(char* filename) : pclfile(filename, ios::in | ios::binary), buflen(1024), buffer(new char[1024]) {
    this->valid = true;

    if (!readPattern(magic,0, 23))
      invalidate("read magic");

    if (!(this->title = readInstrBuffer()))
      invalidate("read title");

    if (!readInstrBuffer())
      invalidate("skip pcl intro");

    PclInstruction* instr;
    while ((instr = readInstr()) && !instr->matches(PCL_END_OF_SETTINGS)) {
      settings[(char*) instr] = instr;
    }

  //  std::map<string, PclInstruction*>::iterator it;
  //  for (it = settings.begin(); it != settings.end(); it++) {
  //    cerr << "param: " << (*it).start << std::endl;
  // }

    if(isSet(PCL_WIDTH))
      this->width = setting(PCL_WIDTH);
    else {
      this->valid = false;
    }

    if(isSet(PCL_HEIGHT))
      this->height = setting(PCL_HEIGHT);
    else {
      this->valid = false;
    }

  };
  virtual ~PclFile(){};

  bool isSet(string signature) {
    return this->settings.find(signature) != settings.end();
  }

  uint32_t setting(string signature) {
    return this->settings[signature]->value;
  }

  void invalidate(string msg) {
    cerr << "invalid pcl file: " << msg << endl;
    this->valid = false;
  }
  bool good() {
    return this->valid && this->pclfile.good();
  }

  PclInstruction* readInstr(const char * expected=NULL) {
    if(!this->pclfile.good()) {
      cerr << "broken pipe" << std::endl;
      return NULL;
    }

    char* token = readInstrBuffer();
    if (token) {
      PclInstruction* instr = new PclInstruction();
      char * endptr;
      char * cursor = token;
      instr->type = *cursor;
      instr->prefix = *(++cursor);
      instr->value = strtol((++cursor), &endptr, 10);

      if (endptr <= cursor) {
        cerr << "failed to parse value:";

        if(expected)
          cerr << expected;

        cerr << "(" << instr->type << instr->prefix << ")" << std::endl;
        return NULL;
      }

      instr->suffix = *endptr;

      if(expected && !instr->matches(expected)) {
        cerr << "expected: " << expected << " skipping: " << (char*)instr << std::endl;
        return NULL;
      } else {
        if (instr->matches(PCL_RLE)) {
          //copy data part of rle instruction
          instr->data = new uint8_t[instr->value];
          memcpy(instr->data, (++endptr), instr->value);
        }
        return instr;
      }
    }

    if(expected) {
      cerr << "failed to read: " << expected << std::endl;
    }
    return NULL;
  }
};

class RLEDecoder {

public:
  RLEDecoder(char* filename): pclfile(filename) {}

  CImg<uint8_t>* decode() {
    if(!this->pclfile.good()) {
      cerr << "Invalid pcl file" << endl;
      return NULL;
    }
    PclInstruction *xInstr, *yInstr, *pixlenInstr, *rleInstr;
    CImg<uint8_t>* img = new CImg<uint8_t>(this->pclfile.width, this->pclfile.height, 1, 1, 255);
     do {
       if(!(yInstr = pclfile.readInstr(PCL_Y)))
         continue;

       if(!(xInstr = pclfile.readInstr(PCL_X)))
         continue;

       if(!(pixlenInstr = pclfile.readInstr(PCL_PIXEL_LEN)))
         continue;

       if(!(rleInstr = pclfile.readInstr(PCL_RLE)))
         continue;

       rleInstr->limit = rleInstr->value;
       this->xoffset = xInstr->value;
       this->slLen = abs(pixlenInstr->value);
       this->reverse = pixlenInstr->value < 0;

       uint32_t pos = 0;
       while ((pos = this->decode(img, rleInstr, pos, yInstr->value))) {}
     } while (this->pclfile.good());

    return img;
  }

private:
  PclFile pclfile;
  uint8_t length;
  uint32_t xoffset;
  uint32_t slLen;
  bool reverse;
  bool fill;

  void init(int rl) {
    if(rl > 128) {
      this->fill = true;
      this->length  = 257 - rl;
    }
    else if(rl < 128) {
      this->fill = false;
      this->length  = rl + 1;
    }
    else {
      this->length = 0;
    }
  }

  uint32_t decode(CImg<uint8_t>* img, PclInstruction* rleInstr, uint32_t pos, uint32_t y) {
    if(!rleInstr->hasNext())
      return 0;

    init(rleInstr->next());

    if(this->length == 0)
      return 0;

    uint8_t intensity = 0;

    if (this->fill && rleInstr->hasNext())
      intensity = 255 - rleInstr->next();
    else
      intensity = 0;

    for (int i = 0; i < this->length; ++i) {
      if (!this->fill)
        intensity = rleInstr->hasNext() ? 255 - rleInstr->next() : 0;

      if(!this->reverse)
        (*img)(xoffset + slLen - (pos + i) - 1, y, 0) = intensity;
      else
        (*img)(xoffset + pos + i, y, 0) = intensity;
    }

    return pos + length;
  }
};

#endif /* PCLFILE_H_ */
