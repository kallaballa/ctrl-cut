/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#define PJL_PARAM
#include <vector>
#include "Document.hpp"
#include "util/Measurement.hpp"
#include "util/Util.hpp"
#include "config/CutSettings.hpp"
#include "cut/operations/Translate.hpp"
#include "CtrlCutException.hpp"
#include "boost/filesystem.hpp"
#include "svg/Svg2Ps.hpp"
#include "svg/CtrlCutParser.hpp"
#include <cstdio>
#include <cstring>
#include "cut/operations/Merge.hpp"

using boost::format;
using std::list;

namespace bfs = boost::filesystem;

typedef EngraveSettings ES;
typedef DocumentSettings DS;
typedef CutSettings CS;

Document::Document(const Document& other) {
  docSettings = other.docSettings;
  const CutList& c = other.cuts();
  Cut* cut;
  Engraving* eng;
  for (CutConstIt it = c.begin(); it != c.end(); ++it) {
    cut = newCut().get();
    (*cut) = *(*it).get();
  }

  const EngraveList& e = other.engravings();
  for (EngraveConstIt it = e.begin(); it != e.end(); ++it) {
    eng = newEngraving().get();
    (*eng) = *(*it).get()->clone();
  }
}

void Document::optimize() {
  for(CutPtr cut : cuts()) {
    cut->normalize();
    cut->sort();
    cut->translate();
  }
}

void Document::mergeCuts() {
  CutList newCuts;
  for(CutPtr cut1 : cuts()) {
    CutPtr copy = CutPtr(new Cut(cut1->settings));
    (*copy.get()) = (*cut1.get());
    if(newCuts.empty()) {
      newCuts.push_back(copy);
      continue;
    }

    bool merged = false;
    for(CutPtr newCut : newCuts) {
      if(cut1 == newCut)
        continue;

      if(cut1->settings == newCut->settings) {
        merge(*copy, *newCut);
        merged = true;
        break;
      }
    }
    if(!merged)
      newCuts.push_back(copy);
  }
  this->cutList = newCuts;
}

void Document::push_back(CutPtr cut) {
  this->cutList.push_back(cut);
}

void Document::push_back(EngravingPtr raster) {
  this->engraveList.push_back(raster);
}

void Document::remove(CutPtr cut) {
  this->cutList.remove(cut);
}

void Document::remove(EngravingPtr engraving) {
  this->engraveList.remove(engraving);
}

Document::Format Document::guessFileFormat(const string& filename) {
  string base = basename(strdup(filename.c_str()));
  string suffix = base.substr(base.rfind(".") + 1);
  transform ( suffix.begin(), suffix.end(), suffix.begin(), &Util::lower_case );

  if (suffix == "vector")
    return VECTOR;
  else if (suffix == "pbm")
    return PBM;
  else if (suffix == "svg")
    return SVG;
  else if (suffix == "cut")
    return CTRLCUT;
  else
    return POSTSCRIPT;
}

std::pair<Document::CutList, Document::EngraveList> Document::load(const string& filename, Format docFormat) {
  CutList newCuts;
  EngraveList newEngravings;

  if(docFormat == UNSPECIFIED)
    docFormat = guessFileFormat(filename);

  string base = basename(strdup(filename.c_str()));

  this->put(DS::DATA_DIR, string(dirname(strdup(filename.c_str()))));
  this->put(DS::FILENAME, filename);

  bool loadCut = this->get(DS::LOAD_CUT);
  bool loadEngraving = this->get(DS::LOAD_ENGRAVING);

  FILE* input_file = NULL;
  FileParser *parser = NULL;

  if(docFormat == CTRLCUT) {
    CtrlCutParser parser;
    parser.load(filename, *this, newCuts, newEngravings);
  } else {
    if (docFormat == POSTSCRIPT || docFormat == SVG) {
      if (docFormat == SVG) {
        int convertPipe[2];

     /*   if(!bfs::exists(filename))
          CtrlCutException::fileNotFoundException(filename);*/

        FILE *svgIn = fopen(filename.c_str(), "r");
        int svgFd = fileno (svgIn);

        if (pipe(convertPipe)) {
          CtrlCutException::generalError("Svg converter pipe failed");
        }

        Svg2Ps converter(svgFd, convertPipe[1]);
        boost::thread svg_converter_thread(&Svg2Ps::convert, converter);

        if ((input_file = fdopen(convertPipe[0], "r")) == NULL) {
          CtrlCutException::generalError("unable to open print file:" + filename);
        }
      } else if(docFormat == POSTSCRIPT){
        if ((input_file = fopen(filename.c_str(), "r")) == NULL) {
          CtrlCutException::generalError("unable to open print file:" + filename);
        }
      }

      string fname = filename;
      string file_basename = Util::make_temp_filename();

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
          CtrlCutException::generalError("Can't open" + filename_cups_debug);
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

      PostscriptParser *psparser = new PostscriptParser(this->settings());
      // Uncomment this to force ghostscript to render to file using the ppmraw
      // backend, instead of in-memory rendering
      //    psparser->setRenderToFile(true);
      if (loadEngraving) {
        switch (this->get(ES::DITHERING)) {
        case ES::BAYER:
        case ES::FLOYD_STEINBERG:
        case ES::JARVIS:
        case ES::BURKE:
        case ES::STUCKI:
        case ES::SIERRA2:
        case ES::SIERRA3:
          psparser->setRasterFormat(PostscriptParser::GRAYSCALE);
          break;

        default:
          assert(false);
          break;
        }
      }
      if (!psparser->parse(input_file)) {
        CtrlCutException::generalError("Error parsing postscript");
      }
      else {
        parser = psparser;
      }
    }

    if (loadEngraving) {
      EngravingPtr engraving(new Engraving(this->settings()));
      if (docFormat == PBM) {
        assert(false);
  /*      std::string suffix = filename.substr(filename.rfind(".") + 1);
        engraving->setImage(loadpbm(filename));
        engraving->put(E_SET::EPOS, Point(392, 516));*/
      }
      else if (parser) {
        if (parser->hasImageData()) {
          LOG_DEBUG_STR("Processing bitmap data from memory");

          if(parser->hasGrayscaleImage()) {
            GrayscaleImage gs = parser->getGrayscaleImage();
            Rectangle cropbox = parser->getCropBox();
  /*          Dither& dither = Dither::create(gs, this->get(E_SET::DITHERING));
            BitmapImage bm = dither.dither(this->get(E_SET::EPOS));*/
            engraving->setImage(gs);
            engraving->put(ES::EPOS, Point(cropbox.ul[0], cropbox.ul[1]));
          } else if(parser->hasBitmapImage()) {
            assert(false);
  /*          Rectangle cropbox = parser->getCropBox();
            engraving->setImage(parser->getBitmapImage());
            engraving->put(E_SET::EPOS, Point(cropbox.ul[0], cropbox.ul[1]));*/
          }
        }
        else if (!parser->getBitmapFile().empty()) {
          std::string suffix = parser->getBitmapFile().substr(parser->getBitmapFile().rfind(".") + 1);
          if (suffix == "ppm" || suffix == "pgm") {
            GrayscaleImage gs = loadppm(filename);
            engraving->setImage(gs);
          }
          else {
            assert(false);
            //engraving->setImage(loadpbm(filename));
          }

          engraving->put(ES::EPOS, Point(392, 516));
        }
      }
      if (engraving && engraving->isAllocated()) {
        this->push_back(engraving);
        newEngravings.push_back(engraving);
      }
    }

    CutPtr cut = NULL;
    if (loadCut) {
      if (docFormat == VECTOR) {
        cut = CutPtr(new Cut(settings()));
        cut->load(filename);
      }
      else if (parser) {
        cut = CutPtr(new Cut(settings()));
        cut->load(parser->getVectorData());
      }

      if (cut)  {
        this->push_back(cut);
        newCuts.push_back(cut);
      }
    }
  }
  return std::make_pair(newCuts,newEngravings);
}

void Document::toJson(std::ostream& os) {
  os << "{ \"doc\": {" << std::endl;
  os << "\"settings\": " << std::endl;
  this->settings().toJson(os);
  os << "," << std::endl << "{ \"data\":" << std::endl << "{" ;
  os << std::endl << " \"cutList\":" << std::endl << "[";

  for (CutList::const_iterator it = cutList.begin(); it != cutList.end();
      ++it) {
    if (it != cutList.begin())
      os << ", " << std::endl;
    (*it)->toJson(os);
  }

  os << "]," << std::endl << "\"engraveList\":" << std::endl << "[";

  for (EngraveList::const_iterator it = engraveList.begin(); it != engraveList.end(); ++it) {
    if (it != engraveList.begin())
      os << ", " << std::endl;
    (*it)->toJson(os);
  }

  os << "] } } }" << std::endl;
}
