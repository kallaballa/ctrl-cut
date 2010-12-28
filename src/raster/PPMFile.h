#ifndef PPMFILE_H_
#define PPMFILE_H_

#include <iostream>
#include <fstream>
#include "MMapImage.h"
using namespace std;

class PPMFile {
public:
  PPMFile();
  virtual ~PPMFile(){};

  MMapImage load(string filename);
  bool skip(ifstream& in, char * name, int v);
  bool skipSpace(ifstream& in);
  int consumeSpace(ifstream& in);
  bool check(char * name, int val);
  int readMagic(ifstream& in);
  int readNum(ifstream& in, int maxdigits);
  void echo(char * c, int val);
  void error(int line, char * c, int val);
  void debug(int line, char * c, int val);
  void debug(char * c, int val[]);
};

#endif /* PPMFILE_H_ */
