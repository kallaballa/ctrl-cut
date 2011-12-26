#include <fstream>
#include "CutModel.h"
#include "Document.h"
#include "util/Logger.h"
#include "cut/geom/Segment.h"

CutModel::CutModel(Document& parent) :
  parent(&parent),
  settings(parent.getSettings()),
  clipped(0),
  zerolength(0)
{ }
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
          this->create(lx, ly, mx, my);
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
          this->create(lx, ly, x, y);
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

void CutModel::remove(Segment& seg) {
  segmentIndex.remove(seg);
}

CutModel::iterator CutModel::erase(SegmentList::iterator it) {
  return  segmentIndex.erase(it);
}

void CutModel::clear() {
  segmentIndex.clear();
}

bool CutModel::create(const Point&  p1, const Point&  p2, const Segment& seg) {
  // ignore zero length segments
  if (p1 == p2) {
    this->zerolength++;
    return false;
  }

  create(Segment(p1,p2,seg));
  return true;
}

bool CutModel::create(const Point&  p1, const Point&  p2) {
  // ignore zero length segments
  if (p1 == p2) {
    this->zerolength++;
    return false;
  }

  create(Segment(p1,p2,*this));
  return true;
}

bool CutModel::create(Segment& seg, bool doclip) {
  if(doclip)
    clip(seg);
  if (seg.first == seg.second) {
    this->clipped++;
    return false;
  }
  create(seg);
  return true;
}

bool CutModel::create(const Segment& seg) {
  segmentIndex.push_back(seg);
  return true;
}


bool CutModel::create(const int32_t& inX,const int32_t& inY,const int32_t& outX,const int32_t& outY) {
  return create(Point(inX, inY), Point(outX, outY));
}


void CutModel::push_front(const Segment& seg) {
  segmentIndex.push_front(seg);
}

void CutModel::push_back(const Segment& seg) {
  segmentIndex.push_back(seg);
}

void CutModel::insert(iterator pos, const Segment& seg) {
  segmentIndex.insert(pos,seg);
}

void CutModel::clip(Segment& seg) {
  typedef DocumentSettings ds;
  int resolution = this->get(ds::RESOLUTION);
  double width = this->get(ds::WIDTH).in(PX, resolution);
  double height = this->get(ds::HEIGHT).in(PX, resolution);

  Segment leftBedBorder(Point(0, 0),Point(0, height-1), *this);
  Segment bottomBedBorder(Point(0, height-1),Point(width-1, height-1), *this);
  Segment rightBedBorder(Point(width-1, height-1),Point(width-1, 0), *this);
  Segment topBedBorder(Point(width-1, 0),Point(0, 0), *this);

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
        seg.first = intersection;
      else
        seg.second = seg.first;
        seg.first = intersection;

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
        seg.first = intersection;
      else
        seg.second = seg.first;
        seg.first = intersection;

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
        seg.first = intersection;
      else
        seg.second = seg.first;
        seg.first = intersection;

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
        seg.first = intersection;
      else
        seg.second = seg.first;
        seg.first = intersection;
    }
    this->clipped++;
  }
}

void CutModel::operator=(const CutModel& other) {
  this->copy(other);
}

void CutModel::copy(const CutModel& other) {
  if(other.hasParent())
    this->parent = other.getParent();

  this->settings = other.getSettings();

  for(CutModel::const_iterator it = other.begin(); it != other.end(); it++) {
    this->create(*it);
  }
}
