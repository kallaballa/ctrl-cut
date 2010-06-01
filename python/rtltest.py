#!/usr/bin/env python

#
# Test script for converting the vector parts of Epilog binary prn files
# (actually RTL files) to SVG.
#
#

import os
import re
import sys
from pysvg import structure
from pysvg import shape
from pysvg import builders
import itertools

def handleYP(power):
    print "YP " + power

def handleZS(speed):
    print "ZS " + speed

def handleXR(freq):
    print "XR " + freq

def handlePU(pos):
    print "PU " + pos
    if "strokelist" in globals() and strokelist != None:
        strokestr = startpos + " "
        for p in strokelist:
            strokestr += "%s,%s " % (p[0], p[1])
        b = builders.ShapeBuilder()
        pl = b.createPolyline(strokestr)
        mySVG.addElement(pl)
    if len(pos) > 0:
        global startpos
#        startpos = parsePosition(pos)
        startpos = pos
#        print "Currpos: " + str(startpos)

def handleLTPU(pos):
    print "LT",
    handlePU(pos[2:])

def handlePD(pointstr):
    print "PD " + pointstr[0:30] + " ..."
    coordlist = pointstr.split(',')
    # Group each pair of coordinates
    global strokelist
    strokelist = list(itertools.izip(*[itertools.islice(coordlist, i, None, 2) for i in range(2)]))

# Returns a list with position [x,y]
def parsePosition(pos):
    return pos.split(',')

# Returns a list of positions [x0, y0, x1, y1 ...]
def parsePointlist(pointlist):
    return pointlist.split(',')

def parseCommand(buffer, pos, end):
    # Extract the command and args
    ro = re.compile("([a-zA-Z]+)([0-9,]*);")
    m = ro.match(buffer, pos, end+1)

    # Call handler function
    funcname = "handle%s" % m.group(1)
    if funcname in globals():
        globals()[funcname](m.group(2))
    else:
        print "Error: Unknown command '" + m.group(1) + "'"

def usage():
    print >> sys.stderr, "Usage: " + sys.argv[0] + " <prn-file>"
    sys.exit(1)

if __name__ == "__main__":

    if len(sys.argv) != 2: usage()
    rtlfile = sys.argv[1]
    print "Processing " + rtlfile + "..."
    global mySVG
    mySVG = structure.svg(rtlfile)

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

    # Export svg to same position as input file
    mySVG.save(os.path.splitext(rtlfile)[0]+".svg")
