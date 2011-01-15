#!/usr/bin/env python

#
# Test script for displaying some info about RTL files
#
#

import os
import re
import sys
import getopt
import itertools
from vec2 import Vec2
from rtlparser import RTLParser

class Polyline(object):

    def __init__(self, power, speed, freq):
        self.power = power
        self.speed = speed
        self.freq = freq
        self.vertices = []

    def addVertex(self, v):
        self.vertices.append(v)

    def numVertices(self):
        return len(self.vertices)

    def length(self):
        lastv = self.vertices[0]
        length = 0
        for v in self.vertices[1:]:
            length += (v - lastv).length()
            lastv = v
        return length

def handleYP(power):
    global currpower
    currpower = power

def handleZS(speed):
    global currspeed
    currspeed = speed

def handleXR(freq):
    global currfreq
    currfreq = freq

def handlePU(pos, strokestr):
    global polylines, pd_was_seen
    if pd_was_seen:
        polyline = Polyline(currpower, currspeed, currfreq)
        for p in [coord.split(",") for coord in strokestr.split(" ") if len(coord) > 0]:
            polyline.addVertex(Vec2(int(p[0]), int(p[1])))
        polylines.append(polyline)

def handlePD(pointstr):
    global pd_was_seen
    pd_was_seen = True;

def usage():
    print >> sys.stderr, "Usage: " + sys.argv[0] + " [<options>] <prn-file>"
    print >> sys.stderr, "Options:"
    print >> sys.stderr, "  -v           Verbose"
    print >> sys.stderr, "  -c           Crop result"

if __name__ == "__main__":

    try:
        opts, args = getopt.getopt(sys.argv[1:], "vc", ["verbose", "crop"])
    except getopt.GetoptError, err:
        usage()
        sys.exit(2)

    if len(args) != 1:
        usage()
        sys.exit(2)

    global verbose, crop
    verbose = False
    crop = False

    for o, a in opts:
        if o in ("-v", "--verbose"): verbose = True
        elif o in ("-c", "--crop"): crop = True

    rtlfile = args[0]

    if verbose: print "Processing " + rtlfile + "..."

    global polylines
    polylines = []

    global pd_was_seen
    pd_was_seen = False;
    parser = RTLParser(verbose=verbose)
    parser.registerHandler("YP", handleYP)
    parser.registerHandler("ZS", handleZS)
    parser.registerHandler("XR", handleXR)
    parser.registerHandler("PU", handlePU)
    parser.registerHandler("PD", handlePD)
    parser.parse(rtlfile)

    totlength = 0
    for i in range(len(polylines)):
        pl = polylines[i]
        length = pl.length()
        totlength += length
        if verbose: print "%d: %.2f (%d)" % (i, length, pl.numVertices())
    print "polylines: %d" % (len(polylines))
    print "length: %.2f" % (totlength * 25.4 / 600) 
    print "boundingbox: %s" % (" ".join(map(str, sum(parser.bbox, []))))
