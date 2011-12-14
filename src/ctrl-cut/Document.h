#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include<libgen.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>
#include "stdint.h"
#include <iostream>
#include "boost/format.hpp"

#include "util/PJL.h"
#include "engrave/Engrave.h"
#include "cut/HPGLEncoder.h"
#include "engrave/PclEncoder.h"
#include "cut/model/CutModel.h"
#include "config/DocumentSettings.h"
#include "config/EngraveSettings.h"

#include "util/Util.h"
#include <cups/cups.h>
#include <cups/file.h>

#include "FileParser.h"
#include <boost/thread.hpp>
#include "svg/Svg2Ps.h"

using std::string;

class Document {
public:
  enum DocumentFormat {
    POSTSCRIPT,
    SVG,
    VECTOR,
    PBM
  };

  typedef std::list<CutModel*> CutList;
  typedef std::list<Engraving*> EngraveList;
  typedef CutList::iterator CutIt;
  typedef EngraveList::iterator EngraveIt;
  typedef CutList::const_iterator CutConstIt;
  typedef EngraveList::const_iterator EngraveConstIt;

  DocumentSettings settings;
  CutList cutList;
  EngraveList engraveList;

  CutIt begin_cut() { return this->cutList.begin(); }
  CutConstIt begin_cut() const  { return this->cutList.begin(); }
  CutIt end_cut() { return this->cutList.end(); }
  CutConstIt end_cut() const  { return this->cutList.end(); }
  CutList::reference front_cut() { return this->cutList.front(); }
  CutList::reference back_cut() { return this->cutList.back(); }
  size_t size_cut() const { return this->cutList.size(); }
  bool empty_cut() const { return this->cutList.empty(); }

  EngraveIt begin_engrave() { return this->engraveList.begin(); }
  EngraveConstIt begin_engrave() const  { return this->engraveList.begin(); }
  EngraveIt end_engrave() { return this->engraveList.end(); }
  EngraveConstIt end_engrave() const  { return this->engraveList.end(); }
  EngraveList::reference front_engrave() { return this->engraveList.front(); }
  EngraveList::reference back_engrave() { return this->engraveList.back(); }
  size_t size_engrave() const { return this->engraveList.size(); }
  bool empty_engrave() const { return this->engraveList.empty(); }

  Document() {};
  virtual ~Document() {};

  void optimize();
  void addCut(CutModel* cut);
  void addRaster(Engraving* raster);
  CutList getCuts() { return cutList; }

  void write(std::ostream &out);
  void preprocess();

  bool load(const string& filename) {
    typedef DocumentSettings DS;

    string filename_vector;
    string filename_pbm;
    DocumentFormat docFormat;
    this->settings.put(DS::DATA_DIR, string(dirname(strdup(filename.c_str()))));
    string base = basename(strdup(filename.c_str()));

    string suffix = base.substr(base.rfind(".") + 1);
    transform ( suffix.begin(), suffix.end(), suffix.begin(), &Util::lower_case );

    this->settings.put(DS::BASENAME,base.erase(base.rfind(".")));
    cups_file_t* input_file;

    if (suffix == "vector") {
      docFormat = VECTOR;
      filename_vector = filename;
    } else if (suffix == "pbm") {
      docFormat = PBM;
      filename_pbm = filename;
    } else if (suffix == "svg") {
      // Try to open the print file...
      docFormat = SVG;
      int convertPipe[2];
      FILE *svgIn = fopen(filename.c_str(), "r");
      int svgFd = fileno (svgIn);

      if (pipe(convertPipe)) {
        fprintf(stderr, "Pipe failed.\n");
        return false;
      }

      Svg2Ps converter(svgFd, convertPipe[1]);
      boost::thread svg_converter_thread(&Svg2Ps::convert, converter);

      if ((input_file = cupsFileOpenFd(convertPipe[0], "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", filename.c_str());
        return false;
      }
    } else {
      docFormat = POSTSCRIPT;
      // Try to open the print file...
      if ((input_file = cupsFileOpen(filename.c_str(), "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", filename.c_str());
        return false;
      }
    }


    FileParser *parser = NULL;

    if (docFormat == POSTSCRIPT || docFormat == SVG) {
      string file_basename = this->settings.get(DS::TEMP_DIR)+ "/" + this->settings.get(DS::BASENAME);

      // Write out the incoming cups data if debug is enabled.
      // FIXME: This is disabled for now since it has a bug:
      // If we're reading from network/stdin, and debug is on, we reopen
      // the dumped file as a FILE*. Otherwise, we'll keep the cups_file_t.
      // Subsequent code doesn't handle the difference.
  #if 0
      FILE *file_debug;
      FILE *file_cups;
      string filename_cups_debug;
      if (cc_loglevel >= CC_DEBUG) {
        /* We save the incoming cups data to the filesystem. */
        filename_cups_debug = file_basename + ".cups";
        file_debug = fopen(filename_cups_debug.c_str(), "w");

        /* Check that file handle opened. */
        if (!file_debug) {
          LOG_FATAL_MSG("Can't open", filename_cups_debug);
          return 1;
        }

        /* Write cups data to the filesystem. */
        int l;
        while ((l = cupsFileRead(input_file, buf, sizeof(buf))) > 0) {
          fwrite(buf, 1, l, file_debug);
        }
        fclose(file_debug);
        /* In case file_cups pointed to stdin we close the existing file handle
         * and switch over to using the debug file handle.
         */
        cupsFileClose(input_file);
        file_cups = fopen(filename_cups_debug, "r");
      }
  #endif

      PostscriptParser *psparser = new PostscriptParser(this->settings);
      // Uncomment this to force ghostscript to render to file using the ppmraw
      // backend, instead of in-memory rendering
      //    psparser->setRenderToFile(true);
      if (this->settings.get(DS::ENABLE_RASTER)) {
        switch (this->settings.get(EngraveSettings::DITHERING)) {
        case EngraveSettings::DEFAULT_DITHERING:
          psparser->setRasterFormat(PostscriptParser::BITMAP);
          break;
        case EngraveSettings::BAYER:
        case EngraveSettings::FLOYD_STEINBERG:
        case EngraveSettings::JARVIS:
        case EngraveSettings::BURKE:
        case EngraveSettings::STUCKI:
        case EngraveSettings::SIERRA2:
        case EngraveSettings::SIERRA3:
          psparser->setRasterFormat(PostscriptParser::GRAYSCALE);
          break;

        default:
          assert(false);
        }
      }
      if (!psparser->parse(input_file)) {
        LOG_FATAL("Error processing postscript");
        return 1;
      }
      else {
        parser = psparser;
      }
    }

    if (this->settings.get(DS::ENABLE_RASTER)) {
      Engraving *raster = NULL;
      if (docFormat == PBM) {
        raster = new Engraving(filename_pbm, this->settings);
      }
      else if (parser) {
        if (parser->hasBitmapData()) {
          LOG_DEBUG_STR("Processing bitmap data from memory");
          raster = new Engraving(parser->getImage(), this->settings);
        }
        else if (!parser->getBitmapFile().empty()) {
          raster = new Engraving(parser->getBitmapFile(), this->settings);
        }
        else {
          LOG_FATAL("No bitmap available from FileParser");
          return 1;
        }
      }
      if (raster) {
        this->addRaster(raster);
      }
    }

    CutModel *cut = NULL;
    if (this->settings.get(DS::ENABLE_VECTOR)) {
      if (docFormat == VECTOR) {
        cut = new CutModel(this->settings);
        if(!cut->load(filename_vector))
          return false;
      }
      else if (parser) {
        cut = new CutModel(this->settings);
        if(!cut->load(parser->getVectorData()))
            return false;
      }
      if (cut) this->addCut(cut);
    }

    return true;
  }
};

#endif /* DOCUMENT_H_ */
