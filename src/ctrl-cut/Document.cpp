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
#include <list>
#include "Document.h"
#include "util/Measurement.h"
#include "cut/model/Explode.h"
#include "cut/model/Reduce.h"
#include "cut/graph/Traverse.h"
#include "encoder/HPGLEncoder.h"
#include "encoder/PclEncoder.h"

using boost::format;
using std::list;

void Document::addCut(CutModel* cut) {
  this->cutList.push_back(cut);
}

void Document::addRaster(Engraving* raster) {
  this->engraveList.push_back(raster);
}

typedef EngraveSettings ES;
typedef DocumentSettings DS;
void Document::write(std::ostream &out) {
  string title = this->get(DS::TITLE);
  int resolution = this->get(DS::RESOLUTION);
  int raster_power = 0;
  int raster_speed = 0;
  bool enable_raster = this->get(DocumentSettings::ENABLE_RASTER);
  bool enable_vector = this->get(DocumentSettings::ENABLE_VECTOR);

  if(enable_raster && !this->engraveList.empty()) {
    raster_power = this->front_engrave()->settings.get(ES::EPOWER);
    raster_speed = this->front_engrave()->settings.get(ES::ESPEED);
  }

  double width = this->get(DocumentSettings::WIDTH).in(PX, resolution);
  double height = this->get(DocumentSettings::HEIGHT).in(PX, resolution);
  int focus = this->get(DocumentSettings::AUTO_FOCUS);

  /* Print the printer job language header. */
  out << format(PJL_HEADER) % title;
  /* Set autofocus on or off. */
  out << format(PCL_AUTOFOCUS) % focus;
  /* FIXME unknown purpose. */
  out << PCL_UNKNOWN_BLAFOO;
  /* FIXME unknown purpose. */
  out << PCL_UNKNOWN_BLAFOO2;
  /* Left (long-edge) offset registration.  Adjusts the position of the
   * logical page across the width of the page.
   */
  out << format(PCL_OFF_X) % 0;
  /* Top (short-edge) offset registration.  Adjusts the position of the
   * logical page across the length of the page.
   */
  out << format(PCL_OFF_Y) % 0;
  /* Resolution of the print. */
  out << format(PCL_PRINT_RESOLUTION) % resolution;
  /* X position = 0 */
  out << format(PCL_POS_X) % 0;
  /* Y position = 0 */
  out << format(PCL_POS_Y) % 0;
  /* PCL resolution. */
  out << format(PCL_RESOLUTION) % resolution;

  // Raster Orientation
  out << format(R_ORIENTATION) % 0;

  // Raster power
  out << format(R_POWER) % raster_power;

  // Raster speed
  out << format(R_SPEED) % raster_speed;

  out << PCL_UNKNOWN_BLAFOO3;
  out << format(R_HEIGHT) % height;
  out << format(R_WIDTH) % width;
  // Raster compression
  int compressionLevel = 2;

  out << format(R_COMPRESSION) % compressionLevel;

  /* If raster power is enabled and raster mode is not 'n' then add that
   * information to the print job.
   */

  if (enable_raster && !this->engraveList.empty()) {
    for (EngraveIt it = this->engraveList.begin(); it != this->engraveList.end(); it++) {
      PclEncoder::encode(out, **it);
    }
  }

  if (enable_vector && !this->cutList.empty()) {
    // FIXME: This is to emulate the LT bug in the Epilog drivers:
    // Check if any clipping has been done in any of the passes, and
    // inject the stray "LT" string. This has no function, just for bug compatibility
    // of the output files. See corresponding FIXME in LaserJob.cpp.
    /* REFACTOR
    for (CutIt it = this->cutList.begin(); it != this->cutList.end(); it++) {
      CutModel *cut = *it;
      if (cut && cut->wasClipped())
        out << "LT";
    }*/

    out << PCL_SECTION_END;

    /* We're going to perform a vector print. */
    for (CutIt it = this->cutList.begin(); it != this->cutList.end(); it++) {
      HPGLEncoder::encode(out,**it);
    }
  }
  out << PCL_SECTION_END << HPGL_PEN_UP;
  out << PCL_RESET;
  out << PJL_FOOTER;

  /* Pad out the remainder of the file with spaces and 'footer'. */
  for (int i = 0; i < 4092; i++) {
    out << ' ';
  }
  out << "Mini]";
}

Document& Document::preprocess() {
   for (CutIt it = this->begin_cut(); it != this->end_cut(); it++) {
     CutModel& model = **it;
     dump("input.txt", model.begin(), model.end());

     CutModel exploded(model);
     MultiSegmentView sv(model);
     explode_segments(SegmentAppendIterator<CutModel>(exploded), sv.begin(), sv.end());
     dump("exploded.txt", exploded.begin(), exploded.end());

     CutModel reduced(model);
     reduce(reduced, exploded.begin(), exploded.end());
     dump("reduced.txt", reduced.begin(), reduced.end());

     model = reduced;
     dump("after-copy.txt", model.begin(), model.end());
   }

   for (EngraveIt it = this->engraveList.begin(); it != this->engraveList.end(); it++) {
     (*it)->dither();
   }

   return *this;
}

Document::Format Document::findFormat(const string& filename) {
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

bool Document::load(const string& filename, LoadType load, Format docFormat) {
  typedef DocumentSettings DS;
  if(docFormat == UNSPECIFIED)
    docFormat = findFormat(filename);

  string base = basename(strdup(filename.c_str()));

  this->put(DS::DATA_DIR, string(dirname(strdup(filename.c_str()))));
  this->put(DS::BASENAME,base.erase(base.rfind(".")));

  cups_file_t* input_file;
  FileParser *parser = NULL;

  if (docFormat == POSTSCRIPT || docFormat == SVG) {
    if (docFormat == SVG) {
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
    } else if(docFormat == POSTSCRIPT){
      if ((input_file = cupsFileOpen(filename.c_str(), "r")) == NULL) {
        LOG_FATAL_MSG("unable to open print file", filename.c_str());
        return false;
      }
    }
    string file_basename = this->get(DS::TEMP_DIR)+ "/" + this->get(DS::BASENAME);

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

    PostscriptParser *psparser = new PostscriptParser(this->getSettings());
    // Uncomment this to force ghostscript to render to file using the ppmraw
    // backend, instead of in-memory rendering
    //    psparser->setRenderToFile(true);
    if (load == ENGRAVING || load == BOTH) {
      switch (this->get(EngraveSettings::DITHERING)) {
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

  if (load == ENGRAVING || load == BOTH) {
    Engraving *raster = NULL;
    if (docFormat == PBM) {
      raster = new Engraving(filename, *this);
    }
    else if (parser) {
      if (parser->hasBitmapData()) {
        LOG_DEBUG_STR("Processing bitmap data from memory");
        raster = new Engraving(*parser->getImage(), *this);
      }
      else if (!parser->getBitmapFile().empty()) {
        raster = new Engraving(parser->getBitmapFile(), *this);
      }
    }
    if (raster) {
      this->addRaster(raster);
    }
  }

  CutModel *cut = NULL;
  if (load == CUT || load == BOTH) {
    if (docFormat == VECTOR) {
      cut = new CutModel(this->settings);
      if(!cut->load(filename))
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
