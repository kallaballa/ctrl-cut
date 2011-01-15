#!/usr/bin/env python

#
# Script for converting the vector parts of Epilog binary prn files
# (actually RTL files) to SVG.
#
#

import os
import re
import sys
import getopt
import itertools
from pysvg import structure
from pysvg import shape
from pysvg import builders
from rtlparser import RTLParser

def handlePU(pos, strokestr):
    b = builders.ShapeBuilder()
    pl = b.createPolyline(strokestr)
    mySVG.addElement(pl)

def usage():
    print >> sys.stderr, "Usage: " + sys.argv[0] + " [<options>] <prn-file> [<svg-file>]"
    print >> sys.stderr, "Options:"
    print >> sys.stderr, "  -v           Verbose"
    print >> sys.stderr, "  -c           Crop result"

if __name__ == "__main__":

    try:
        opts, args = getopt.getopt(sys.argv[1:], "vc", ["verbose", "crop"])
    except getopt.GetoptError, err:
        usage()
        sys.exit(2)

    if len(args) < 1 or len(args) > 2:
        usage()
        sys.exit(2)

    global verbose, crop
    verbose = False
    crop = False

    for o, a in opts:
        if o in ("-v", "--verbose"): verbose = True
        elif o in ("-c", "--crop"): crop = True

    rtlfile = args[0]
    if len(args) == 2:
        svgfile = args[1]
    else:
        svgfile = os.path.splitext(rtlfile)[0]+".svg"

    if verbose: print "Processing " + rtlfile + "..."
    global mySVG
    mySVG = structure.svg(rtlfile, width=21600, height=14400)

    parser = RTLParser(verbose=verbose)
    parser.registerHandler("PU", handlePU)
    parser.parse(rtlfile)

    if verbose: print "BBox: " + str(parser.bbox)

    if crop:
        mySVG.set_preserveAspectRatio("xMidYMid meet")
        width,height = parser.bbox[1][0]-parser.bbox[0][0], parser.bbox[1][1]-parser.bbox[0][1]
        mySVG.set_viewBox(str(parser.bbox[0][0]) + " " + str(parser.bbox[0][1]) + " " + str(width) + " " + str(height))
        mySVG.set_width(width)
        mySVG.set_height(height)

    # Export svg to same position as input file
    mySVG.save(svgfile)
