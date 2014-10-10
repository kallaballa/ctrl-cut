#ifndef SVG_TO_PS_H_
#define SVG_TO_PS_H_


#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <boost/thread.hpp>

//QT makes use of signals too
#undef signals
#include <gio/gunixinputstream.h>
#include <librsvg/rsvg.h>

#include <cairo-ps.h>
#include <cairo-pdf.h>
#include "SvgFix.hpp"

#define FAIL(msg)							\
    do { fprintf (stderr, "FAIL: %s\n", msg); exit (-1); } while (0)

#define PIXELS_PER_POINT 1

class Svg2Ps {
private:
  int fdIn, fdOut;
  bool doFixSvg;
public:

  Svg2Ps(int fdIn, int fdOut, bool doFixSvg=true) : fdIn(fdIn), fdOut(fdOut), doFixSvg(doFixSvg) { }
  void convert();

};

#endif /* SVG_TO_PS_H_ */
