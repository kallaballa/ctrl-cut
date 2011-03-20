#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <string>
#include <cups/cups.h>
#include <cups/file.h>
#include "util/LaserConfig.h"
#include <string>
#include <fstream>

class FileParser
{
public:
  FileParser(LaserConfig &conf) : conf(conf) {}
  virtual ~FileParser() {}

  //  bool parse(const string &filename) = 0;
  virtual std::istream &getVectorData() = 0;
  virtual const std::string &getBitmapFile() = 0;

protected:
  LaserConfig &conf;
};

class PostscriptParser : public FileParser
{
public:
  PostscriptParser(LaserConfig &conf) : FileParser(conf) {}
  ~PostscriptParser();

  bool parse(cups_file_t *ps_file);
  // enable API
  // enable raster
  // enable vector
  // use driver: ppmraw, display
  std::istream &getVectorData();
  const std::string &getBitmapFile() {return filename_bitmap;}

private:
  bool createEps(cups_file_t *input_file, const std::string &filename_eps);

  std::string filename_eps;
  std::string filename_bitmap;
#ifndef USE_GHOSTSCRIPT_API
  std::string filename_vector;
  std::ifstream vectorfile;
#endif
};

#endif
