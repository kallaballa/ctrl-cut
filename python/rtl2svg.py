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

def handleYP(power):
    if verbose: print "YP " + power
    pass

def handleZS(speed):
    if verbose: print "ZS " + speed
    pass

def handleXR(freq):
    if verbose: print "XR " + freq
    pass

def expandBBox(x, y):
    global bbox
    if x < bbox[0][0]: bbox[0][0] = x
    if x > bbox[1][0]: bbox[1][0] = x
    if y < bbox[0][1]: bbox[0][1] = y
    if y > bbox[1][1]: bbox[1][1] = y

def handlePU(pos):
    if verbose: print "PU " + pos
    global currpos, startpos, bbox
    if "strokelist" in globals() and strokelist != None:
        if verbose: print("new stroke")
        strokelist.insert(0, startpos)
        strokestr = ""
        for p in strokelist:
            expandBBox(int(p[0]), int(p[1]))
            strokestr += "%s,%s " % (p[0], p[1])
#        if verbose: print(strokestr)
        b = builders.ShapeBuilder()
        pl = b.createPolyline(strokestr)
        mySVG.addElement(pl)
    if len(pos) > 0: currpos = parsePosition(pos)
    startpos = None

def handleLTPU(pos):
    if verbose: print "LT " + pos,
    handlePU(pos)

def handlePD(pointstr):
    if verbose: 
        print "PD " + pointstr[0:30],
        if len(pointstr) > 30: print "..."
        else: print
    global startpos
    startpos = currpos
    coordlist = pointstr.split(',')
    # Group each pair of coordinates
    global strokelist
    strokelist = list(itertools.izip(*[itertools.islice(coordlist, i, None, 2) for i in range(2)]))
    # FIXME: We don't support subsequent PD commands without a PU in between

# Returns a list with position [x,y]
def parsePosition(pos):
    return pos.split(',')

# Returns a list of positions [x0, y0, x1, y1 ...]
def parsePointlist(pointlist):
    return pointlist.split(',')

def parseCommand(buffer, pos, end):
    # Extract the command and args
    if verbose:
        pend = end
        if end - pos > 30: pend = pos + 30
        if verbose: print "cmd: " + buffer[pos:pend],
        if end - pos > 30: print "..."
        else: print
    ro = re.compile("([a-zA-Z]+)([0-9,]*);")
    m = ro.match(buffer, pos, end+1)

    # Call handler function
    funcname = "handle%s" % m.group(1)
    if funcname in globals():
        globals()[funcname](m.group(2))
    else:
        print "Error: Unknown command '" + m.group(1) + "'"

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
    global bbox
    bbox = [[21600, 14400],[0, 0]]

    f = open(rtlfile, "rb")
    buffer = f.read()

    # Look for start of HP-GL/2 section
    pos = buffer.find("\x1b%1BIN;");
    if pos > 0: pos += 7

    while pos < len(buffer):
        # This signifies the end of HP-GL/2 section
        if buffer[pos] == '\x1b': break
        end = buffer.find(";", pos)
        if end <= pos:
            print "Error: Unexpected ';'"
            exit(1)

        parseCommand(buffer, pos, end)
        pos = end + 1

    # FIXME: Assert that there aren't more HP-GL/2 sections

    # Implicit PU when ending
    handlePU("");

    if crop:
        mySVG.set_preserveAspectRatio("xMidYMid meet")
        mySVG.set_viewBox(" ".join(map(str, sum(bbox, []))))
        mySVG.set_width(bbox[1][0]-bbox[0][0])
        mySVG.set_height(bbox[1][1]-bbox[0][1])

    # Export svg to same position as input file
    mySVG.save(svgfile)
