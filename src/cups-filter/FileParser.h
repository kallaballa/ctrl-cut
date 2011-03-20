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

  // Bitmap methods
  virtual bool hasBitmapData() = 0;
  virtual const std::string &getBitmapFile() = 0;
  virtual void *getBitmapData() = 0;
  virtual int getBitmapWidth() = 0;
  virtual int getBitmapHeight() = 0;

protected:
  LaserConfig &conf;
};

class PostscriptParser : public FileParser
{
public:
  PostscriptParser(LaserConfig &conf);
  ~PostscriptParser();

  void setRasterDriver(const char *drivername) {this->rasterdriver = drivername;}
  bool parse(cups_file_t *ps_file);
  // enable API
  // enable raster
  // enable vector
  // use driver: ppmraw, display
  std::istream &getVectorData();

  virtual bool hasBitmapData() {return (this->bitmapdata != NULL);}
  const std::string &getBitmapFile() {return filename_bitmap;}

  void *allocBitmap(unsigned long size);
  void copyPage();

  void *getBitmapData();
  void setBitmapSize(int width, int height, int bytes_per_pixel);
  int getBitmapWidth() {return this->bitmapwidth;}
  int getBitmapHeight() {return this->bitmapheight;}

  void setGhostScriptBuffer(void *pimage) {this->gsbuffer = pimage;}
  void printStatistics();

  // FIXME: Singleton for now since ghostscript callbacks don't provide a userdata pointer.
  // Fix this with a lookup table or smth.
  static PostscriptParser *instance() { return PostscriptParser::inst; }
  static PostscriptParser *inst;
private:
  bool createEps(cups_file_t *input_file, const std::string &filename_eps);

  std::string rasterdriver;
  std::string filename_eps;
  std::string filename_bitmap;
#ifndef USE_GHOSTSCRIPT_API
  std::string filename_vector;
  std::ifstream vectorfile;
#else
  void *gsbuffer;
  void *bitmapdata;
  unsigned long bitmapsize;
  int bitmapwidth;
  int bitmapheight;
  int bytesperpixel;
#endif
};

#endif
