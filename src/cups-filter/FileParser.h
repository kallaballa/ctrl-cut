#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <string>
#include <cups/cups.h>
#include <cups/file.h>
#include <string>
#include <fstream>
#include <vector>

#include "util/LaserConfig.h"
#include "raster/Image.h"

class FileParser
{
public:
  FileParser(LaserConfig &conf) : conf(conf) {}
  virtual ~FileParser() {}

  //  bool parse(const string &filename) = 0;
  virtual std::istream &getVectorData() = 0;

  // Bitmap methods
  virtual bool hasBitmapData() = 0;
  virtual CCImage *getImage() = 0;
  virtual const std::string &getBitmapFile() = 0;

protected:
  LaserConfig &conf;
};

class PostscriptParser : public FileParser
{
public:
  PostscriptParser(LaserConfig &conf);
  ~PostscriptParser();

  void setRenderToFile(bool enable) {this->rendertofile = enable;}
  void setComponents(uint8_t components) {this->components = components;}
  uint8_t getComponents() const {return this->components;}

  bool parse(cups_file_t *ps_file);
  std::istream &getVectorData();

  virtual bool hasBitmapData() {return (this->image != NULL);}
  const std::string &getBitmapFile() {return filename_bitmap;}

  void copyPage();

  void createImage(uint32_t width, uint32_t height, void *pimage);
  CCImage *getImage() {return this->image;}

  void printStatistics();

  // FIXME: Singleton for now since ghostscript callbacks don't provide a userdata pointer.
  // Fix this with a lookup table or smth.
  static PostscriptParser *instance() { return PostscriptParser::inst; }
  static PostscriptParser *inst;
private:
  bool createEps(cups_file_t *input_file, const std::string &filename_eps);

  std::string filename_eps;
  std::string filename_bitmap;
#ifndef USE_GHOSTSCRIPT_API
  bool execute_ghostscript_cmd(const std::vector<std::string> &argstrings)

  std::string filename_vector;
  std::ifstream vectorfile;
#else
  bool execute_ghostscript(const std::vector<std::string> &argstrings);

  bool rendertofile;
  uint8_t components;

  CCImage *gsimage;
  CCImage *image;

#endif
};

#endif
