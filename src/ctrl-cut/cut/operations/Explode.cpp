#include "Explode.hpp"
#include "math.h"

#ifdef CC_EXPLODE_DEBUG

void printSegment(const Segment& seg, size_t indent) {
  for(;indent > 0; --indent)
    std::cerr << "\t";

  std::cerr << seg.first.x << ","
      << seg.first.y << " "
      << seg.second.x << ","
      << seg.second.y << std::endl;
}
void debug(const Segment& pick) {
  if(Segment(Point(1005,12387), Point(1005,13798)) == pick)
    std::cerr << "";
  printSegment(pick, 0);
}

void debug(const Segment& pick, const Segment& candidate) {
  printSegment(candidate, 1);
}

void debug(const Segment& pick, const Segment& candidate, const Point& intersection) {
  std::cerr << "\t\t" << intersection.x << "," << intersection.y << std::endl;
}

void debugEmitted(const Segment& s0) {
  printSegment(s0,3);
}

void debugTree(IndexedSegmentTree& tree) {
  std::cerr << std::endl;
  std::cerr << std::endl;
  for(IndexedSegmentTree::iterator it = tree.begin(); it != tree.end(); ++it) {
    debug(*it);
  }
  std::cerr << std::endl;
  std::cerr << std::endl;
}

void checkStableSlope(const Segment& pick, const Segment& candidate,
    const Point& intersection) {
  float sp = pick.getSlope();
  float sp1;

  if (pick[0] == intersection)
    sp1 = Segment(pick[1], intersection).getSlope();
  else
    sp1 = Segment(pick[0], intersection).getSlope();

  float sc = candidate.getSlope();
  float sc1;

  if (candidate[0] == intersection)
    sc1 = Segment(candidate[1], intersection).getSlope();
  else
    sc1 = Segment(candidate[0], intersection).getSlope();

  assert(abs(sp - sp1) <= 0.01 || abs(sc - sc1) <= 0.01);
}

void checkStableSlopeEmitted(const Segment& s0, const Segment& s1, const Segment& s2) {
   float ss0 = s0.getSlope();
   float ss1 = s1.getSlope();
   float ss2 = s2.getSlope();

   assert(abs(abs(ss0 - ss1) - ss2) <= 0.02);
}

#else

void printSegment(const Segment& seg, size_t indent) {}
void debug(const Segment& pick) {}
void debug(const Segment& pick, const Segment& candidate) {}
void debug(const Segment& pick, const Segment& candidate, const Point& intersection) {}
void debugEmitted(const Segment& s0) {}
void debugTree(IndexedSegmentTree& tree) {}
void checkStableSlope(const Segment& pick, const Segment& candidate, const Point& intersection) {}
void checkStableSlopeEmitted(const Segment& s0, const Segment& s1, const Segment& s2) {}

#endif

