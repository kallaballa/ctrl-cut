#
# Utility functions for parsing RTL files
#

import sys
import re
import itertools

# Returns a list with position [x,y]
def parsePosition(pos):
    return pos.split(',')

# Returns a list of positions [x0, y0, x1, y1 ...]
def parsePointlist(pointlist):
    return pointlist.split(',')

class RTLParser:
    def __init__(self, verbose=False):
        self.verbose = verbose
        self.reset()
        self.handlers = {}

    def reset(self):
        self.bbox = [[sys.maxint, sys.maxint],[0, 0]]
        self.startpos = self.currpos = None
        self.strokelist = None

    def registerHandler(self, primitive, func):
        self.handlers[primitive] = func

    def parse(self, rtlfile):
        f = open(rtlfile, "rb")
        buffer = f.read()

        # Look for start of HP-GL/2 section
        pos = buffer.find("\x1b%1BIN;")
        if pos > 0: pos += 7

        hpglend = buffer.find('\x1b%0B', pos)
        if hpglend == -1:
            print "Error: End of HP/GL-2 section no found"
            exit(1)

        prevpos = pos;
        while pos < hpglend:
            # This signifies the end of HP-GL/2 section
            if buffer[pos] == '\x1b%0B': break
            end = buffer.find(";", pos, hpglend)
            if end == -1:
                print "Error: Command not terminated: '" + buffer[pos:hpglend] + "'"
                print "       Previous command: '" + buffer[prevpos:pos] + "'"
                exit(1)
            if end == pos:
                print "Error: Unexpected ';' at '" + buffer[pos-10:pos+1]
                exit(1)

            self.parseCommand(buffer, pos, end)
            prevpos = pos
            pos = end + 1

        # FIXME: Assert that there aren't more HP-GL/2 sections
            
        # Implicit PU when ending
        self.handlePU("");

    def parseCommand(self, buffer, pos, end):
        # Extract the command and args
        if self.verbose:
            pend = end
            if end - pos > 30: pend = pos + 30
            if self.verbose: print "cmd: " + buffer[pos:pend],
            if end - pos > 30: print "..."
            else: print
        ro = re.compile("([a-zA-Z]+)([0-9,]*);")
        m = ro.match(buffer, pos, end+1)

        # Call handler function
        funcname = "handle%s" % m.group(1)
        if hasattr(self, funcname):
            getattr(self, funcname)(m.group(2))
        else:
            print "Error: Unknown command '" + m.group(1) + "'"

    def expandBBox(self, x, y):
        if x < self.bbox[0][0]: self.bbox[0][0] = x
        if x > self.bbox[1][0]: self.bbox[1][0] = x
        if y < self.bbox[0][1]: self.bbox[0][1] = y
        if y > self.bbox[1][1]: self.bbox[1][1] = y

    def handleYP(self, power):
        if self.verbose: print "YP " + power
        if "YP" in self.handlers:
            self.handlers["YP"](power)

    def handleZS(self, speed):
        if self.verbose: print "ZS " + speed
        if "ZS" in self.handlers:
            self.handlers["ZS"](speed)

    def handleXR(self, freq):
        if self.verbose: print "XR " + freq
        if "XR" in self.handlers:
            self.handlers["XR"](freq)

    def handlePU(self, pos):
        if self.verbose: print "PU " + pos
        if self.strokelist != None:
            if self.verbose: print("new stroke")
            self.strokelist.insert(0, self.startpos)
            strokestr = ""
            for p in self.strokelist:
                self.expandBBox(int(p[0]), int(p[1]))
                strokestr += "%s,%s " % (p[0], p[1])
#            if self.verbose: print(strokestr)a
            if "PU" in self.handlers:
                self.handlers["PU"](pos, strokestr)
        if len(pos) > 0: self.currpos = parsePosition(pos)
        self.startpos = None

    def handleLTPU(self, pos):
        if self.verbose: print "LT " + pos,
        if "LTPU" in self.handlers:
            self.handlers["LTPU"](pos)
        self.handlePU(pos)

    def handlePD(self, pointstr):
        if self.verbose: 
            print "PD " + pointstr[0:30],
            if len(pointstr) > 30: print "..."
            else: print
        self.startpos = self.currpos
        coordlist = pointstr.split(',')
        # Group each pair of coordinates
        self.strokelist = list(itertools.izip(*[itertools.islice(coordlist, i, None, 2) for i in range(2)]))
        # FIXME: We don't support subsequent PD commands without a PU in between
        if "PD" in self.handlers:
            self.handlers["PD"](pointstr)
