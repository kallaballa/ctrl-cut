#include "CutModel.h"
#include "cut/geom/Geometry.h"
#include "cut/graph/Traverse.h"
#include "util/Logger.h"
#include "util/Measurement.h"
#include "config/DocumentSettings.h"
#include <fstream>

DocumentSettings CutModel::defaultDocSettings;


/*!
 Loads vector data from EPS/Ghostscript output
 */
bool CutModel::load(std::istream &input) {
  std::string line;
  char first;
  int power, x, y;
  int lx = 0, ly = 0;
  int mx = 0, my = 0;
  LOG_INFO_STR("Load vector data");
  int segmentCnt = 0;
  while (std::getline(input, line)) {
    first = line[0];

    if (first == 'X') { // End of output
      break;
    }

    if (isalpha(first)) {
      switch (first) {
      case 'M': // move to
        if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
          lx = x;
          ly = y;
          mx = x;
          my = y;
        }
        break;
      case 'C': // close
        if (lx != mx || ly != my) {
          segmentCnt++;
          this->createSegment(lx, ly, mx, my, OpParams(power, 0, 0));
        }
        break;
      case 'P': // power
        if (sscanf(line.c_str() + 1, "%d", &x) == 1) {
          // FIXME: While testing, ignore the strange color-intensity-is-power convension
          //          power = x;
          power = -1;
        }
        break;
      case 'L': // line to
        if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
          segmentCnt++;
          this->createSegment(lx, ly, x, y, OpParams(power, 0, 0));
          lx = x;
          ly = y;
        }
        break;
      }
    }
  }

  LOG_DEBUG_MSG("zero length segments", this->zerolength);
  LOG_DEBUG_MSG("clipped segments", this->clipped);
  LOG_DEBUG_MSG("loaded segments", segmentCnt);

  if (this->empty()) {
    return false;
  }

  return true;
}

/*!
 Loads vector data from EPS/Ghostscript output from the given file
 */
bool CutModel::load(const std::string &filename) {
  std::ifstream infile(filename.c_str(), std::ios_base::in);
  return this->load(infile);
}

void make_route(StringList& route, CutModel& model) {
  const Point&  pos = model.settings.get(CutSettings::CPOS);
  SegmentList::iterator segmentsFirst;
  SegmentList::iterator segmentsLast;
  SegmentList translated;

  if(pos.x != 0 || pos.y != 0) {
    translate(translated, model.begin(), model.end(), pos);
    segmentsFirst = translated.begin();
    segmentsLast = translated.end();
  } else{
    segmentsFirst = model.begin();
    segmentsLast = model.end();
  }

  CutSettings::Optimize optimize = model.settings.get(CutSettings::OPTIMIZE);
  string datadir = model.settings.get(DocumentSettings::DATA_DIR);
  string basename = model.settings.get(DocumentSettings::BASENAME);

  if(optimize == CutSettings::SIMPLE) {
    //FIXME dump_linestrings("simple", segmentsFirst, segmentsLast);
  } else if(optimize == CutSettings::SHORTEST_PATH) {
    StringList join;
    make_linestrings(join,segmentsFirst, segmentsLast);
    travel_linestrings(route, join.begin(), join.end());

    //FIXME dump_linestrings("join", segmentsFirst, segmentsLast);
  } else if(optimize == CutSettings::INNER_OUTER) {
    traverse_onion(route, segmentsFirst, segmentsLast);

    //FIXME dump_linestrings("innerouter", segmentsFirst, segmentsLast);
  } else
    assert(false);
}

void CutModel::add(Segment& seg) {
  clip(seg);
  if (seg.first == seg.second) // ignore zero length segments
    return;
  segmentIndex.push_back(&seg);
}

void CutModel::remove(Segment& seg) {
  segmentIndex.remove(&seg);
  delete &seg;
}

CutModel::iterator CutModel::erase(SegmentList::iterator it) {
  Segment* const seg = *it;
  iterator next = segmentIndex.erase(it);
  delete seg;
  return next;
}

void CutModel::clear() {
  for (iterator it = begin(); it != end(); it++) {
    it = erase(it);
  }
}

bool CutModel::createSegment(const Point&  p1, const Point&  p2, const OpParams& settings) {
  // ignore zero length segments
  if (p1 == p2) {
    this->zerolength++;
    return false;
  }
  add(*new Segment(p1, p2, settings));
  return true;
}

bool CutModel::createSegment(const int32_t& inX,const int32_t& inY,const int32_t& outX,const int32_t& outY,const OpParams& settings) {
  return createSegment(Point(inX, inY), Point(outX, outY), settings);
}


void CutModel::clip(Segment& seg) {
  typedef DocumentSettings ds;
  int resolution = this->settings.get(ds::RESOLUTION);
  double width = this->settings.get(ds::WIDTH).in(PX, resolution);
  double height = this->settings.get(ds::HEIGHT).in(PX, resolution);

  Segment leftBedBorder(Point(0, 0),Point(0, height-1),OpParams(0,0,0));
  Segment bottomBedBorder(Point(0, height-1),Point(width-1, height-1), OpParams(0,0,0));
  Segment rightBedBorder(Point(width-1, height-1),Point(width-1, 0),OpParams(0,0,0));
  Segment topBedBorder(Point(width-1, 0),Point(0, 0),OpParams(0,0,0));

  Point intersection;
  Segment clipped;

  if(seg.first.x < 0 || seg.second.x < 0) {
    // out of bounds;
    if(seg.first.x < 0 && seg.second.x < 0) {
      this->clipped++;
      return;
    }

    if(intersects(seg, leftBedBorder, intersection) == ALIGN_INTERSECT) {
      if(seg.first.x < seg.second.x)
        seg = Segment(intersection, seg.second, seg.settings);
      else
        seg = Segment(intersection, seg.first, seg.settings);

      intersection = Point();
      this->clipped++;
    }
  }

  if(seg.first.y < 0 || seg.second.y < 0) {
    if(seg.first.y < 0 && seg.second.y < 0) {
      this->clipped++;
    }

    if(intersects(seg, topBedBorder, intersection) == ALIGN_INTERSECT) {
      if(seg.first.y < seg.second.y)
        seg = Segment(intersection, seg.second, seg.settings);
      else
        seg = Segment(intersection, seg.first, seg.settings);

      intersection = Point();
      this->clipped++;
    }
  }


  if(greater_than(seg.first.x,width - 1) || greater_than(seg.second.x,width - 1)) {
    if(greater_than(seg.first.x, width - 1) && greater_than(seg.second.x,width - 1)) {
      this->clipped++;
      return;
    }

    if(intersects(seg, rightBedBorder, intersection) == ALIGN_INTERSECT) {
      if(seg.first.x > seg.second.x)
        seg = Segment(intersection, seg.second, seg.settings);
      else
        seg = Segment(intersection, seg.first, seg.settings);

      intersection = Point();
      this->clipped++;
    }
  }

  if(greater_than(seg.first.y, height - 1) || greater_than(seg.second.y,height - 1)) {
    if(greater_than(seg.first.y, height - 1) && greater_than(seg.second.y,height - 1)) {
      this->clipped++;
      return;
    }
    if(intersects(seg, bottomBedBorder, intersection) == ALIGN_INTERSECT) {
      if(seg.first.y > seg.second.y)
        seg = Segment(intersection, seg.second, seg.settings);
      else
        seg = Segment(intersection, seg.first, seg.settings);
    }
    this->clipped++;
  }
}
