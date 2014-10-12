
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
