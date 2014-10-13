/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Svg2Ps.hpp"
#include "cut/Cut.hpp"

using std::ofstream;

FILE* fpOut;

cairo_status_t cairowrite(void *closure, const unsigned char *data,
    unsigned int length) {
  if (fwrite(data, 1, length, fpOut) < length)
    return CAIRO_STATUS_WRITE_ERROR;
  return CAIRO_STATUS_SUCCESS;
}

void Svg2Ps::convert() {
  int svgFixPipe[2];
  if (pipe(svgFixPipe)) {
    fprintf(stderr, "Pipe failed.\n");
  }

  SvgFix svgFix(fdIn, svgFixPipe[1]);
  boost::thread svg_fix_thread(&SvgFix::work, svgFix);

  GError *error = NULL;
  RsvgHandle *handle;
  RsvgDimensionData dim;
  double width, height;

  cairo_surface_t *surface;
  cairo_t *cr;
  cairo_status_t status;

  g_type_init();

  rsvg_set_default_dpi(72);

  handle = rsvg_handle_new_from_stream_sync(g_unix_input_stream_new(svgFixPipe[0], TRUE), NULL, RSVG_HANDLE_FLAGS_NONE, NULL, &error);

  if (error != NULL)
    FAIL (error->message);

  rsvg_handle_get_dimensions(handle, &dim);

  width = dim.width;
  height = dim.height;

  fpOut = fdopen(fdOut,"w");

  surface = cairo_ps_surface_create_for_stream(
      (cairo_write_func_t) &cairowrite, NULL, width, height);
  cr = cairo_create(surface);

  rsvg_handle_render_cairo(handle, cr);

  status = cairo_status(cr);
  if (status)
    FAIL (cairo_status_to_string (status));

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  svg_fix_thread.join();
  fflush(fpOut);
  fclose(fpOut);
}
