/*
 * Ctrl-Cut - A laser cutter CUPS driver
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

#ifndef CUPSGETOPS_H_
#define CUPSGETOPS_H_

#include <stdlib.h>
#include <string>
#include <iostream>
#include <cups/cups.h>
#include "util/Logger.hpp"
#include "Document.hpp"
#include "CupsOptions.hpp"

using std::string;
using std::cerr;
using std::endl;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class CupsGetOpt {
public:
  bool dumpXML;
  string jobid;
  string user;
  string title;
  string copies;
  string options;
  string filename;

  CupsGetOpt() : dumpXML(false) {}
  virtual ~CupsGetOpt() {}

  static void printEnv(const string &env) {
    const char *envstr = getenv(env.c_str());
    if (envstr) {
      LOG_DEBUG_MSG(env, envstr);
    }
  }

  static int lower_case(int c) {
    return tolower (c);
  }

  void dumpEnv() const;

  static void print_usage(const string &name);
  static CupsOptions load_document(Document& doc, int argc, char *argv[]);
};

#endif /* CUPSGETOPS_H_ */
