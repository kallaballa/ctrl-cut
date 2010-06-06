#include "eps_converter.h"
#include "laser_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/**
 * Convert the given postscript file (ps) converting it to an encapsulated
 * postscript file (eps).
 *
 * @param ps_file a file handle pointing to an opened postscript file that
 * is to be converted.
 * @param eps_file a file handle pointing to the opened encapsulated
 * postscript file to store the result.
 *
 * @return Return true if the function completes its task, false otherwise.
 */
bool ps_to_eps(laser_config *lconf, FILE *ps_file, FILE *eps_file)
{
  int xoffset = 0;
  int yoffset = 0;

  int l;
  while (fgets((char *) buf, sizeof(buf), ps_file)) {
    fprintf(eps_file, "%s", (char *) buf);
    if (*buf != '%') {
      break;
    }
    if (!strncasecmp((char *) buf, "%%BoundingBox:", 14)) {
      int lower_left_x;
      int lower_left_y;
      int upper_right_x;
      int upper_right_y;
      if (sscanf((char *) buf + 14, "%d %d %d %d", &lower_left_x,
                 &lower_left_y, &upper_right_x, &upper_right_y) == 4) {
        xoffset = lower_left_x;
        yoffset = lower_left_y;
        lconf->width = (upper_right_x - lower_left_x);
        lconf->height = (upper_right_y - lower_left_y);
        fprintf(eps_file, "/setpagedevice{pop}def\n"); // use bbox
        fprintf(eps_file, "0 %d translate\n", lconf->height
                - upper_right_y);
        if (xoffset || yoffset) {
          fprintf(eps_file, "%d %d translate\n", -xoffset, -yoffset);
        }
        if (lconf->flip) {
          fprintf(eps_file, "%d 0 translate -1 1 scale\n",
                  lconf->width);
        }
      }
    }
    if (!strncasecmp((char *) buf, "%!", 2)) {
      fprintf(eps_file,
              "/==={(        )cvs print}def/stroke{currentlinewidth matrix "
              "currentmatrix 0 get mul 5 lt {(P)=== currentrgbcolor pop pop 100 mul "
              "round  cvi = flattenpath{transform(M)=== 1 sub round cvi ===(,)=== 1 "
              "sub round cvi =}{transform(L)=== 1 sub round cvi ===(,)=== 1 sub round "
              "cvi =}{}{(C)=}pathforall newpath}{stroke}ifelse}bind def/showpage{(X)= "
              "showpage}bind def\n");
      if (lconf->raster_mode != 'c' && lconf->raster_mode != 'g') {
        if (lconf->screen == 0) {
          fprintf(eps_file, "{0.5 ge{1}{0}ifelse}settransfer\n");
        } else {
          int s = lconf->screen;
          if (s < 0) {
            s = 0 - s;
          }
          if (lconf->resolution >= 600) {
            // adjust for overprint
            fprintf(eps_file,
                    "{dup 0 ne{%d %d div add}if}settransfer\n",
                    lconf->resolution / 600, s);
          }
          fprintf(eps_file, "%d 30{%s}setscreen\n", lconf->resolution
                  / s, (lconf->screen > 0) ? "pop abs 1 exch sub"
                  : "180 mul cos exch 180 mul cos add 2 div");
        }
      }
    }
  }
  while ((l = fread((char *) buf, 1, sizeof(buf), ps_file)) > 0) {
    fwrite((char *) buf, 1, l, eps_file);
  }
  return true;
}
