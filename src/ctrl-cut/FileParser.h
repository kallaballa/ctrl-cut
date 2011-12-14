#ifndef FILEPARSER_H_
#define FILEPARSER_H_

#include <string>
#include <cups/cups.h>
#include <cups/file.h>
#include <string>
#include <fstream>
#include <vector>

#include "config/DocumentSettings.h"
#include "engrave/Image.h"

class FileParser
{
public:
  FileParser(DocumentSettings &conf) : conf(conf) {}
  virtual ~FileParser() {}

  //  bool parse(const string &filename) = 0;
  virtual std::istream &getVectorData() = 0;

  // Bitmap methods
  virtual bool hasBitmapData() = 0;
  virtual AbstractImage *getImage() = 0;
  virtual const std::string &getBitmapFile() = 0;

protected:
  DocumentSettings &conf;
};

class PostscriptParser : public FileParser
{
public:
  PostscriptParser(DocumentSettings &conf);
  ~PostscriptParser();

  enum RasterFormat {
    BITMAP,              // 1 bit per pixel
    GRAYSCALE,           // 8 bits per pixel
    RGB                  // 24 bits per pixel
  };

  void setRasterFormat(RasterFormat format) {this->rasterformat = format;}

  void setRenderToFile(bool enable) {this->rendertofile = enable;}
  void setComponents(uint8_t components) {this->components = components;}
  uint8_t getComponents() const {return this->components;}

  bool parse(cups_file_t *ps_file);
  std::istream &getVectorData();

  virtual bool hasBitmapData() {return (this->image != NULL);}
  const std::string &getBitmapFile() {return filename_bitmap;}

  void copyPage();

  void createImage(uint32_t width, uint32_t height, void *pimage, uint32_t rowstride = 0);
  AbstractImage *getImage() {return this->image;}

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
  RasterFormat rasterformat;
  uint8_t components;

  AbstractImage *gsimage;
  AbstractImage *image;

#endif
};

#endif
