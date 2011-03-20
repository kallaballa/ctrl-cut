#include "FileParser.h"
#include "util/Logger.h"
#include "util/Eps.h"
#include <cups/cups.h>
#include <cups/file.h>

#ifdef USE_GHOSTSCRIPT_API
#include <sstream>
#include <ghostscript/iapi.h>
#include <ghostscript/ierrors.h>
#include <ghostscript/gdevdsp.h>
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

PostscriptParser *PostscriptParser::inst = NULL;

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
 * @param rasterdriver a string which contains the ghostscript driver to use (ppmraw, display, nullpage)
 * @param resolution the encapsulated postscript resolution.
 * @param height the postscript height in points per inch.
 * @param width the postscript width in points per inch.
 *
 * @return Return true if the execution of ghostscript succeeds, false
 * otherwise.
 */
#ifdef USE_GHOSTSCRIPT_API
static int display_open(void *handle, void *device)
{
  LOG_DEBUG("display_open()");
  return 0;
}

static int display_preclose(void *handle, void *device)
{
  LOG_DEBUG("display_preclose()");
  return 0;
}

static int display_close(void *handle, void *device)
{
  LOG_DEBUG("display_close()");
  return 0;
}

static int display_presize(void *handle, void *device, int width, int height, 
                           int raster, unsigned int fmt)
{
  LOG_DEBUG(str(format("display_presize(%d,%d)") % width % height));
  return 0;
}
   
static int display_size(void *handle, void *device, int width, int height, 
                        int raster, unsigned int fmt, unsigned char *pimage)
{
  LOG_DEBUG(str(format("display_size(%d,%d)") % width % height));
  fprintf(stderr, "    pimage: %p\n", pimage);

  int color = fmt & DISPLAY_COLORS_MASK;
  int depth = fmt & DISPLAY_DEPTH_MASK;
  assert(color == DISPLAY_COLORS_RGB);
  assert(depth == DISPLAY_DEPTH_8);
  // FIXME: Take note of raster (=rowstride)

  PostscriptParser::instance()->setBitmapSize(width, height, 3);
  PostscriptParser::instance()->setGhostScriptBuffer(pimage);

  return 0;
}
   
static int display_sync(void *handle, void *device)
{
  LOG_DEBUG("display_sync()");

  return 0;
}

/*!
  Called by ghostscript when a page has finished rendering.
  NB! The memory buffer will be cleared to background color by ghostscript after
  this function has returned.
*/
static int display_page(void *handle, void *device, int copies, int flush)
{
  LOG_DEBUG("display_page()");

  PostscriptParser::instance()->copyPage();

  return 0;
}

#if 0
static int display_update(void *handle, void *device, 
                          int x, int y, int w, int h)
{
  LOG_DEBUG(str(format("display_update([ %d , %d , %d , % d])") % x % y % w % h));
  return 0;
}

static void *display_memalloc(void *handle, void *device, unsigned long size)
{
  void *buffer = PostscriptParser::instance()->allocBitmap(size);
  fprintf(stderr, "display_memalloc: %p\n", buffer);
  return buffer;
}

static int display_memfree(void *handle, void *device, void *mem)
{
  return 0;
}
#endif

static int display_separation(void *handle, void *device,
                       int comp_num, const char *name,
                       unsigned short c, unsigned short m,
                       unsigned short y, unsigned short k)
{
  LOG_DEBUG("display_separation()");
  return 0;
}

// callback structure for display device
display_callback display_callbacks = { 
    sizeof(display_callback),
    DISPLAY_VERSION_MAJOR,
    DISPLAY_VERSION_MINOR,
    display_open,
    display_preclose,
    display_close,
    display_presize,
    display_size,
    display_sync,
    display_page,
    NULL, // display_update,
    NULL, // display_memalloc,
    NULL, // display_memfree,
    display_separation
};

bool execute_ghostscript(const char *filename_eps, const char *filename_bitmap,
                         const char *rasterdriver, int resolution, int height,
                         int width) {
  std::vector<std::string> argstrings;
  argstrings.push_back("gs");
  argstrings.push_back("-q");
  argstrings.push_back("-dBATCH");
  argstrings.push_back("-dDEBUG");
  argstrings.push_back("-dNOPAUSE");
  argstrings.push_back(str(format("-r%d") % resolution));
  argstrings.push_back(str(format("-g%dx%d")
                                  % ((width * resolution) / POINTS_PER_INCH)
                                  % ((height * resolution) / POINTS_PER_INCH)));
  argstrings.push_back(str(format("-sDEVICE=%s") % rasterdriver));
  if (!strcmp(rasterdriver, "display")) {
    argstrings.push_back(str(format("-dDisplayFormat=%d") % 
                             (DISPLAY_COLORS_RGB | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
                              DISPLAY_BIGENDIAN | DISPLAY_TOPFIRST)));
    
  }
  else {
    argstrings.push_back(str(format("-sOutputFile=%s") % filename_bitmap));
  }
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
  if (!strcmp(rasterdriver, "display")) {
    code = gsapi_set_display_callback(minst, &display_callbacks);
  }
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
                             const char *filename_vector, const char *rasterdriver, int resolution,
                             int height, int width) {
  char buf[8192];
  sprintf(
      buf,
      "%s -q -dBATCH -dNOPAUSE -r%d -g%dx%d -sDEVICE=%s -sOutputFile=%s %s > %s",
      GS_EXECUTABLE, resolution, (width * resolution) / POINTS_PER_INCH,
      (height * resolution) / POINTS_PER_INCH, rasterdriver, filename_bitmap,
      filename_eps, filename_vector);

  LOG_DEBUG(buf);

  if (system(buf)) {
    return false;
  }

  return true;
}
#endif

PostscriptParser::PostscriptParser(LaserConfig &conf) 
  : FileParser(conf), rasterdriver("ppmraw"), bitmapdata(NULL)
{
  PostscriptParser::inst = this;
}

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
#ifdef USE_GHOSTSCRIPT_API
  if (this->bitmapdata) free(this->bitmapdata);
#endif

  PostscriptParser::inst = NULL;
}

bool PostscriptParser::parse(cups_file_t *input_file)
{
  this->filename_eps = this->conf.tempdir + "/" + this->conf.basename + ".eps";

  if (!createEps(input_file, this->filename_eps)) return false;

#ifdef USE_GHOSTSCRIPT_API
  if (this->rasterdriver != "display") {
    this->filename_bitmap = this->conf.tempdir + "/" + this->conf.basename + ".ppm";
    LOG_DEBUG_MSG("Running ghostscript. Raster output", this->filename_bitmap);
  }
  if (!execute_ghostscript(this->filename_eps.c_str(), this->filename_bitmap.c_str(), 
                           this->conf.enable_raster ? this->rasterdriver.c_str() : "nullpage",
                           this->conf.resolution, this->conf.height, this->conf.width)) {
    LOG_FATAL_STR("ghostscript failed");
    return false;
  }
#else
  this->filename_bitmap = this->conf.tempdir + "/" + this->conf.basename + ".ppm";
  LOG_DEBUG_MSG("Running ghostscript. Raster output", this->filename_bitmap);
  this->filename_vector = this->conf.tempdir + "/" + this->conf.basename + ".vector";
  LOG_DEBUG_MSG("                     Vector output", this->filename_vector);
  if (!execute_ghostscript_cmd(this->filename_eps.c_str(), this->filename_bitmap.c_str(),
                               this->filename_vector.c_str(),
                               this->conf.enable_raster ? this->rasterdriver.c_str() : "nullpage",
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

void *PostscriptParser::getBitmapData()
{
  return this->bitmapdata;
}

void PostscriptParser::setBitmapSize(int width, int height, int bytes_per_pixel)
{
  this->bitmapwidth = width;
  this->bitmapheight = height;
  this->bytesperpixel = bytes_per_pixel;
}


void PostscriptParser::printStatistics()
{
  unsigned int r=0,g=0,b=0;
  for (int j=0;j<this->bitmapheight;j++) {
    for (int i=0;i<this->bitmapwidth;i++) {
      uint8_t *buf = (uint8_t *)this->bitmapdata;
      if (buf[(j*bitmapwidth+i)*3] != 255) 
        r++;
      if (buf[(j*bitmapwidth+i)*3+1] != 255) g++;
      if (buf[(j*bitmapwidth+i)*3+2] != 255) b++;
    }
  }
  LOG_DEBUG(r);
  LOG_DEBUG(g);
  LOG_DEBUG(b);
}

void PostscriptParser::copyPage()
{
  size_t size = this->bitmapwidth * this->bitmapheight * this->bytesperpixel;
  this->bitmapdata = malloc(size);
  assert(this->bitmapdata);
  memcpy(this->bitmapdata, this->gsbuffer, size);
}