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

#ifndef HPGL_H_
#define HPGL_H_

#include <limits>
#include <iostream>

using std::ostream;
using std::endl;

#define HPGL_UNSET std::numeric_limits<int32_t>::max()
#define HPGL_INIT "IN"
#define HPGL_LTPENUP "LTPU"
#define HPGL_PENUP "PU"
#define HPGL_PENDOWN "PD"
#define HPGL_MOVE "MOVE"
#define HPGL_END_OF_INSTRUCTION ';'

class HpglInstr {
public:
  string operation;
  int32_t parameters[2];
  off64_t file_off;

  HpglInstr(off64_t file_off) : file_off(file_off) {
    parameters[0] = HPGL_UNSET;
    parameters[1] = HPGL_UNSET;
  }

  bool matches(const string& signature, bool report=false) {
    bool m = strcmp(operation.c_str(), signature.c_str()) == 0;
    if(!m && report)
      cerr << "expected: " << signature << " found: " << this << endl;

    return m;
  }

  //FIXME friend used on a member function?
  friend ostream& operator <<(ostream &os, HpglInstr &instr) {
    os << (format("(%08X) %s") % instr.file_off % instr.operation);

    if(instr.parameters[0] != (int32_t)HPGL_UNSET) {
      os << " = " << instr.parameters[0];
      if(instr.parameters[1] != (int32_t)HPGL_UNSET) {
        os << "," << instr.parameters[1];
      }
    }

    return os;
  }
};

#endif /* HPGL_H_ */
