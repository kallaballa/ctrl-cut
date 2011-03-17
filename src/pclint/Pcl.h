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

#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <list>
#include <iostream>
#include <boost/format.hpp>
#include "Signatures.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>
#include "2D.h"
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ostream;
using std::list;
using boost::format;

// off_t is 64-bit on BSD-derived systems
#ifdef __APPLE__
typedef off_t off64_t;
#endif

#define MAGIC_SIZE 23
const char *MAGIC = "\x1b%-12345X@PJL JOB NAME=";

class PclInstr {
public:
  PclInstr(off64_t file_off) : type(255), prefix(255), suffix(255), keysep('\0'), value(0), pos(0), limit(0), file_off(file_off), hasValue(false), hasData(false) {}
  char type;
  char prefix;
  char suffix;
  char keysep; // separate the instr signature from value/data so it may be interpreted as a string and used as key
  int32_t value;
  uint8_t * data;
  uint16_t pos;
  uint16_t limit;
  off64_t file_off;

  bool hasValue;
  bool hasData;

  bool hasNext() {
    return this->pos < this->limit;
  }

  uint8_t next(){
      return (uint8_t) *(this->data+this->pos++);
  }

  bool matches(const string& signature, bool report=false) {
    bool m = memcmp(this, signature.c_str(), 3) == 0;
    if(!m && report)
      cerr << "expected: " << signature << " found: " << (char*)this << endl;

    return m;
  }

  static const string pretty(char c) {
    if(!isgraph(c))
      return (format("(0x%02X)") % c).str();
    else
      return (format("%c") % c).str();
  }

  friend ostream& operator <<(ostream &os, PclInstr &instr) {
    format fmtInstr("(%08X) %s%s%s = %d");
    fmtInstr % instr.file_off
        % PclInstr::pretty(instr.type)
        % PclInstr::pretty(instr.prefix)
        % PclInstr::pretty(instr.suffix);

    if(instr.hasValue)
      fmtInstr % instr.value;
    else
      fmtInstr % "NULL";

    return os << fmtInstr;
  }
};

class Trace {
private:
  const uint8_t backlogSize;
  list<PclInstr*> backlog;
  static Trace* instance;
  Point penPos;
  Point relPos;

  Trace(): backlogSize(10), penPos(0,0), relPos(0,0) {}
public:
  static Trace* singleton();

  void logInstr(PclInstr* instr) {
    cerr << penPos << relPos << "\t" << *instr << endl;
    if(backlog.size() >= backlogSize)
      backlog.erase(backlog.begin());

    backlog.push_back(instr);
  }

  void logPlotterStat(Point &penPos, Point &relPos) {
    this->penPos = penPos;
    this->relPos = relPos;
  }

  list<PclInstr*>::iterator backlogIterator() {
    return backlog.begin();
  }

  list<PclInstr*>::iterator backlogEnd() {
    return backlog.end();
  }

  void printBacklog(ostream &os, string msg) {
    os << "=== trace: " << msg << ": " << endl;
    if(backlog.empty()){
      os << "(backlog N/A)" << endl;
    }else{
      for(list<PclInstr*>::iterator it = backlogIterator(); it != backlogEnd(); it++)
        os << "\t" << *(*it) << endl;
    }
    os << endl;
  }
};

Trace* Trace::instance = NULL;
Trace* Trace::singleton() {
  if (instance == NULL)
    instance = new Trace();

  return instance;
}

#endif /* PCLFILE_H_ */
