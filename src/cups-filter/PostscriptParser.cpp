#include "FileParser.h"
#include "util/Logger.h"
#include "util/Eps.h"
#include <cups/cups.h>
#include <cups/file.h>

#ifdef USE_GHOSTSCRIPT_API
#include <sstream>
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>
#include "boost/format.hpp"
using boost::str;
using boost::format;
#else
#ifndef __APPLE__
#define GS_EXECUTABLE "/usr/bin/gs"
#else
#define GS_EXECUTABLE "/opt/local/bin/gs"
#endif
#endif

using std::string;

#ifdef USE_GHOSTSCRIPT_API
std::stringstream vectorbuffer;
static int GSDLLCALL
gsdll_stdout(void *, const char *str, int len)
{
  vectorbuffer.write(str, len);
  return len;
}
#endif

/**
 * Execute ghostscript feeding it an ecapsulated postscript file which
 * is then converted into a bitmap image. As a byproduct, output of
 * the ghostscript process is redirected to the global stringstream
 * vectorbuffer, which will contain instructions on how to perform a
 * vector cut of lines within the postscript.
 *
 * @param filename_bitmap the filename to use for the resulting bitmap file.
 * @param filename_eps the filename to read in encapsulated postscript from.
 * @param bmp_mode a string which is one of bmp16m, bmpgray, or bmpmono.
 * @param resolution the encapsulated postscript resolution.
 * @param height the postscript height in points per inch.
 * @param width the postscript width in points per inch.
 *
 * @return Return true if the execution of ghostscript succeeds, false
 * otherwise.
 */
#ifdef USE_GHOSTSCRIPT_API
bool execute_ghostscript(const char *filename_eps, const char *filename_bitmap,
                         const char *bmp_mode, int resolution, int height,
                         int width) {
  std::vector<std::string> argstrings;
  argstrings.push_back("gs");
  argstrings.push_back("-q");
  argstrings.push_back("-dBATCH");
  argstrings.push_back("-dNOPAUSE");
  argstrings.push_back(str(format("-r%d") % resolution));
  argstrings.push_back(str(format("-g%dx%d")
                                  % ((width * resolution) / POINTS_PER_INCH)
                                  % ((height * resolution) / POINTS_PER_INCH)));
  argstrings.push_back(str(format("-sDEVICE=%s") % bmp_mode));
  argstrings.push_back(str(format("-sOutputFile=%s") % filename_bitmap));
  argstrings.push_back(filename_eps);

  int gsargc = argstrings.size();
  const char *gsargv[gsargc];
  for (int i=0;i<gsargc;i++) {
    gsargv[i] = argstrings[i].c_str();
  }
  void *minst;
  int code = gsapi_new_instance(&minst, NULL);
  if (code < 0) {
    LOG_ERR_MSG("gsapi_new_instance() failed", code);
    return false;
  }
  gsapi_set_stdio(minst, NULL, gsdll_stdout, NULL);
  code = gsapi_init_with_args(minst, gsargc, (char **)gsargv);
  int code1 = gsapi_exit(minst);
  if ((code == 0) || (code == e_Quit)) {
    code = code1;
  }

  gsapi_delete_instance(minst);

  if ((code == 0) || (code == e_Quit)) {
    return true;
  }
  return false;
}
#endif

#ifndef USE_GHOSTSCRIPT_API
bool execute_ghostscript_cmd(const char *filename_eps, const char *filename_bitmap,
                             const char *filename_vector, const char *bmp_mode, int resolution,
                             int height, int width) {
  char buf[8192];
  sprintf(
      buf,
      "%s -q -dBATCH -dNOPAUSE -r%d -g%dx%d -sDEVICE=%s -sOutputFile=%s %s > %s",
      GS_EXECUTABLE, resolution, (width * resolution) / POINTS_PER_INCH,
      (height * resolution) / POINTS_PER_INCH, bmp_mode, filename_bitmap,
      filename_eps, filename_vector);

  LOG_DEBUG(buf);

  if (system(buf)) {
    return false;
  }

  return true;
}
#endif


PostscriptParser::~PostscriptParser()
{
  // Cleanup unneeded files provided that debug mode is disabled.
  if (cc_loglevel > CC_DEBUG) {
    if (unlink(this->filename_eps.c_str())) {
      LOG_FATAL_MSG("unlink failed", this->filename_eps);
    }
    if (this->conf.enable_raster) {
      if (unlink(this->filename_bitmap.c_str())) {
        LOG_FATAL_MSG("unlink failed", this->filename_bitmap);
      }
    }
  }
}

bool PostscriptParser::parse(cups_file_t *input_file)
{
  this->filename_eps = this->conf.tempdir + "/" + this->conf.basename + ".eps";
  this->filename_bitmap = this->conf.tempdir + "/" + this->conf.basename + ".ppm";

  if (!createEps(input_file, this->filename_eps)) return false;

  LOG_DEBUG_MSG("Running ghostscript. Raster output", this->filename_bitmap);
#ifdef USE_GHOSTSCRIPT_API
  if (!execute_ghostscript(this->filename_eps.c_str(), this->filename_bitmap.c_str(), 
                           this->conf.enable_raster ? "ppmraw" : "nullpage",
                           this->conf.resolution, this->conf.height, this->conf.width)) {
    LOG_FATAL_STR("ghostscript failed");
    return false;
  }
#else
  this->filename_vector = this->conf.tempdir + "/" + this->conf.basename + ".vector";
  LOG_DEBUG_MSG("                     Vector output", this->filename_vector);
  if (!execute_ghostscript_cmd(this->filename_eps.c_str(), this->filename_bitmap.c_str(),
                               this->filename_vector.c_str(),
                               this->conf.enable_raster ? "ppmraw" : "nullpage",
                               this->conf.resolution, this->conf.height, this->conf.width)) {
    LOG_FATAL_STR("ghostscript failed");
    return false;
  }
#endif
  return true;
}

bool PostscriptParser::createEps(cups_file_t *input_file, const string &filename_eps)
{
  // Open the encapsulated postscript file for writing.
  FILE *file_eps = fopen(filename_eps.c_str(), "w");
  if (!file_eps) {
    LOG_FATAL_MSG("Can't open", filename_eps);
    return false;
  }
  LOG_DEBUG_MSG("Converting to EPS file", filename_eps);
  
  // Convert PS to EPS (for vector extraction)
  if (!ps_to_eps(&this->conf, input_file, file_eps)) {
    LOG_FATAL_STR("ps_to_eps failed");
    fclose(file_eps);
    return false;
  }
  // Cleanup after encapsulated postscript creation.
  fclose(file_eps);
  if (input_file != cupsFileStdin()) cupsFileClose(input_file);

  return true;
}

std::istream &PostscriptParser::getVectorData()
{
#ifdef USE_GHOSTSCRIPT_API
  return vectorbuffer;
#else
  if (this->vectorfile.is_open()) {
    this->vectorfile.seekg(0);
  }
  else {
    this->vectorfile.open(this->filename_vector.c_str(), std::ios_base::in);
  }
  return this->vectorfile;
#endif  
}
