#!/usr/bin/env python

#
# Test script for displaying some info about RTL files
#
#

import os
import re
import sys
import getopt
import math
import itertools
from vec2 import Vec2
from rtlparser import RTLParser
from rtlparser import parsePosition

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

# Pos is the position to move to after PU
# strokestr is the polyline drawn since the last PD
def handlePU(pos, strokestr):
    global polylines, pd_was_seen, lastpos, moves

    # Register non-cutting move
    if pos:
        parsedpos = parsePosition(pos)
        moves.append(lastpos -  Vec2(int(parsedpos[0]), int(parsedpos[1])))

    if pd_was_seen:
        polyline = Polyline(currpower, currspeed, currfreq)
        for p in [coord.split(",") for coord in strokestr.split(" ") if len(coord) > 0]:
            polyline.addVertex(Vec2(int(p[0]), int(p[1])))
        polylines.append(polyline)
        lastpos = polyline.vertices[len(polyline.vertices)-1]

def handlePD(pointstr):
    global pd_was_seen
    pd_was_seen = True;

def usage():
    print >> sys.stderr, "Usage: " + sys.argv[0] + " [<options>] <prn-file> [<prn-file>]"
    print >> sys.stderr, "Options:"
    print >> sys.stderr, "  -v           Verbose"
    print >> sys.stderr, "  -c           Crop result"

class Statistics:
    def __init__(self, polylines, totlength, movement, segments, bbox):
        self.polylines = polylines
        self.totlength = totlength
        self.movement = movement
        self.segments = segments
        self.bbox = bbox

def getStatistics(rtlfile):
    global moves

    parser = RTLParser(verbose=verbose)
    parser.registerHandler("YP", handleYP)
    parser.registerHandler("ZS", handleZS)
    parser.registerHandler("XR", handleXR)
    parser.registerHandler("PU", handlePU)
    parser.registerHandler("PD", handlePD)
    parser.parse(rtlfile)
    totlength = 0
    segments = 0
    for i in range(len(polylines)):
        pl = polylines[i]
        length = pl.length()
        totlength += length
        segments += pl.numVertices() - 1
        if verbose: print "%d: %.2f (%d)" % (i, length, pl.numVertices())

    movement = 0
    for i in range(len(moves)):
        movement += moves[i].length()

    return Statistics(len(polylines), totlength * 25.4 / 600, movement * 25.4 / 600, segments, parser.bbox)


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
    if len(args) > 1: second_rtlfile = args[1]

    if verbose: print "Processing " + rtlfile + "..."

    global polylines
    polylines = []

    global moves
    moves = []

    global lastpos
    lastpos = Vec2(0,0)

    global pd_was_seen
    pd_was_seen = False;

    stats = getStatistics(rtlfile)

    if len(args) == 1:
        print "polylines: %d" % (stats.polylines)
        print "cutting length: %.2f" % (stats.totlength)
        print "movement: %.2f" % (stats.movement)
        print "segments: %d" % (stats.segments)
        print "boundingbox: %s" % (" ".join(map(str, sum(stats.bbox, []))))
    else:
        first_polylines = polylines
        first_moves = moves

        polylines = []
        moves = []
        lastpos = Vec2(0,0)
        pd_was_seen = False;
        stats2 = getStatistics(second_rtlfile)

        fail = False
        polyline_diff = abs(len(first_polylines) - len(polylines))
        if polyline_diff > 1:
            print str(len(first_polylines)) + "->" + str(len(polylines)),
            fail = True
        else:
            print "0",

        totlen_diff = math.fabs(stats.totlength - stats2.totlength)
        if totlen_diff > 1:
            print "%.2f" % (stats.totlength) + "->" + "%.2f" % (stats2.totlength),
            fail = True
        else:
            print "0",

        movement_diff = math.fabs(stats.movement - stats2.movement)
        if movement_diff > 1:
            print "%.2f" % (stats.movement) + "->" + "%.2f" % (stats2.movement),
            fail = True
        else:
            print "0",

        segments_diff = math.fabs(stats.segments - stats2.segments)
        if segments_diff > 1:
            print "%d" % (stats.segments) + "->" + "%d" % (stats2.segments),
            fail = True
        else:
            print "0",

        if abs(stats.bbox[0][0] - stats2.bbox[0][0]) > 1 or \
           abs(stats.bbox[0][1] - stats2.bbox[0][1]) > 1 or \
           abs(stats.bbox[1][0] - stats2.bbox[1][0]) > 1 or \
           abs(stats.bbox[1][1] - stats2.bbox[1][1]) > 1:
            print "1"
            fail = True
        else:
            print "0"

        if fail: sys.exit(1)
