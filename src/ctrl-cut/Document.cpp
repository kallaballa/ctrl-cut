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
#define PJL_PARAM
#include <vector>
#include "Document.hpp"
#include "util/Measurement.hpp"
#include "config/CutSettings.hpp"
#include "cut/operations/Translate.hpp"
#include "CtrlCutException.hpp"
#include "boost/filesystem.hpp"
#include "svg/Svg2Ps.hpp"


using boost::format;
using std::list;

namespace bfs = boost::filesystem3;

typedef EngraveSettings E_SET;
typedef DocumentSettings D_SET;
typedef CutSettings C_SET;

void Document::push_back(Cut* cut) {
  this->cutList.push_back(cut);
}

void Document::push_back(Engraving* raster) {
  this->engraveList.push_back(raster);
}

void Document::remove(Cut* cut) {
  this->cutList.remove(cut);
}

void Document::remove(Engraving* engraving) {
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
  else
    return POSTSCRIPT;
}

bool Document::load(const string& filename, Format docFormat) {
  if(docFormat == UNSPECIFIED)
    docFormat = guessFileFormat(filename);

  string base = basename(strdup(filename.c_str()));

  this->put(D_SET::DATA_DIR, string(dirname(strdup(filename.c_str()))));
  this->put(D_SET::FILENAME, filename);

  bool loadCut = this->get(D_SET::LOAD_CUT);
  bool loadEngraving = this->get(D_SET::LOAD_ENGRAVING);

  cups_file_t* input_file;
  FileParser *parser = NULL;

  if (docFormat == POSTSCRIPT || docFormat == SVG) {
    if (docFormat == SVG) {
      int convertPipe[2];

   /*   if(!bfs::exists(filename))
        CtrlCutException::fileNotFoundException(filename);*/

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
    } else if(docFormat == POSTSCRIPT){
      if ((input_file = cupsFileOpen(filename.c_str(), "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", filename.c_str());
        return false;
      }
    }

    string fname = filename;
    string file_basename = this->get(D_SET::TEMP_DIR)+ "/" + fname.erase(fname.rfind("."));

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

    PostscriptParser *psparser = new PostscriptParser(this->settings());
    // Uncomment this to force ghostscript to render to file using the ppmraw
    // backend, instead of in-memory rendering
    //    psparser->setRenderToFile(true);
    if (loadEngraving) {
      switch (this->get(E_SET::DITHERING)) {
      case E_SET::DEFAULT_DITHERING:
//        psparser->setRasterFormat(PostscriptParser::BITMAP);
        //break;
      case E_SET::BAYER:
      case E_SET::FLOYD_STEINBERG:
      case E_SET::JARVIS:
      case E_SET::BURKE:
      case E_SET::STUCKI:
      case E_SET::SIERRA2:
      case E_SET::SIERRA3:
        psparser->setRasterFormat(PostscriptParser::GRAYSCALE);
        break;

      default:
        assert(false);
        break;
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

  if (loadEngraving) {
    Engraving* engraving = new Engraving(this->settings());
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
          engraving->put(E_SET::EPOS, Point(cropbox.ul[0], cropbox.ul[1]));
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

        engraving->put(E_SET::EPOS, Point(392, 516));
      }
    }
    if (engraving && engraving->isAllocated()) {
      this->push_back(engraving);
    }
  }

  Cut *cut = NULL;
  if (loadCut) {
    if (docFormat == VECTOR) {
      cut = new Cut(this->settings());
      if(!cut->load(filename))
        return false;
    }
    else if (parser) {
      cut = new Cut(this->settings());
      if(!cut->load(parser->getVectorData()))
          return false;
    }
    if (cut)  {
      this->push_back(cut);
      cut->normalize();
    }
  }

  return true;
}
