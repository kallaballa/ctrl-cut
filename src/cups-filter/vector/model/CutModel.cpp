#include "CutModel.h"
#include "util/Logger.h"
#include <fstream>

bool CutModel::createSegment(const Point &p1, const Point &p2,
    CutSettings& settings) {
  if (p1 == p2) // ignore zero length segments
    return false;

  add(*new Segment(p1, p2, settings));
  return true;
}

const Segment& CutModel::clipSegmentToLaserBed(const Segment &unclipped) {
  const Segment* seg = &unclipped;
  Point* intersection = new Point();

  if(seg->first.x < 0 || seg->second.x < 0) {
    if(seg->first.x < 0 && seg->second.x < 0) // out of bounds;
      return (* new Segment(*new Point(), *new Point(), seg->settings));

    if(intersects(*seg, this->leftBedBorder, *intersection) == ALIGN_INTERSECT) {
      if(seg->first.x < seg->second.x)
        seg = new Segment(*intersection, seg->second, seg->settings);
      else
        seg = new Segment(*intersection, seg->first, seg->settings);

      intersection = new Point();
      this->clipped = true;
    }
  }

  if(seg->first.y < 0 || seg->second.y < 0) {
    if(seg->first.y < 0 && seg->second.y < 0)
      return (* new Segment(*new Point(), *new Point(), seg->settings)); // out of bounds;

    if(intersects(*seg, this->topBedBorder, *intersection) == ALIGN_INTERSECT) {
      if(seg->first.y < seg->second.y)
        seg = new Segment(*intersection, seg->second, seg->settings);
      else
        seg = new Segment(*intersection, seg->first, seg->settings);

      intersection = new Point();
      this->clipped = true;
    }
  }

  if(greater_than(seg->first.x,this->config.device_width - 1) || greater_than(seg->second.x,this->config.device_width - 1)) {
    if(greater_than(seg->first.x, this->config.device_width - 1) && greater_than(seg->second.x,this->config.device_width - 1))
      return (* new Segment(*new Point(), *new Point(), seg->settings)); // out of bounds;

    if(intersects(*seg, this->rightBedBorder, *intersection) == ALIGN_INTERSECT) {
      if(seg->first.x > seg->second.x)
        seg = new Segment(*intersection, seg->second, seg->settings);
      else
        seg = new Segment(*intersection, seg->first, seg->settings);

      intersection = new Point();
      this->clipped = true;
    }
  }

  if(greater_than(seg->first.y, this->config.device_height - 1) || greater_than(seg->second.y,this->config.device_height - 1)) {
    if(greater_than(seg->first.y, this->config.device_height - 1) && greater_than(seg->second.y,this->config.device_height - 1))
      return (* new Segment(*new Point(), *new Point(), seg->settings)); // out of bounds;

    if(intersects(*seg, this->bottomBedBorder, *intersection) == ALIGN_INTERSECT) {
      if(seg->first.y > seg->second.y)
        seg = new Segment(*intersection, seg->second, seg->settings);
      else
        seg = new Segment(*intersection, seg->first, seg->settings);
    }
    this->clipped = true;
  }

  return *seg;
}

bool CutModel::createSegment(int32_t inX, int32_t inY, int32_t outX,
    int32_t outY, CutSettings& settings) {
 // std::cerr << "#" << inX << "/" << inY << " " << outX << "/" << outY << std::endl;
  return createSegment(*(new Point(inX, inY)), *(new Point(outX, outY)), settings);
}

void CutModel::add(const Segment& seg) {
  const Segment& clipped = clipSegmentToLaserBed(seg);
  if (clipped.first == clipped.second) // ignore zero length segments
    return;
  iterator it_seg = segmentIndex.insert(end(),&clipped);
  segmentTree.add(it_seg);
}

CutModel::iterator CutModel::remove(iterator it_seg) {
  assert(it_seg != end());
  segmentTree.remove(it_seg);
  return segmentIndex.erase(it_seg);
}

/*!
 Loads vector data from EPS/Ghostscript output
 */
CutModel *CutModel::load(std::istream &input) {
  CutModel *model = new CutModel();
  std::string line;
  char first;
  int power, x, y;
  int lx = 0, ly = 0;
  int mx = 0, my = 0;
  LOG_INFO_STR("Load vector data");
  int segmentCnt = 0;
  while (std::getline(input, line)) {
    std::cerr << line << std::endl;
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
          model->createSegment(lx, ly, mx, my, (*new CutSettings(power, 0, 0)));
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
          model->createSegment(lx, ly, x, y, (*new CutSettings(power, 0, 0)));
          lx = x;
          ly = y;
        }
        break;
      }
    }
  }

  if (model->empty()) {
    delete model;
    model = NULL;
  }

  LOG_DEBUG_MSG("loaded segments", segmentCnt);

  return model;
}

/*!
 Loads vector data from EPS/Ghostscript output from the given file
 */
CutModel *CutModel::load(const std::string &filename) {
  std::ifstream infile(filename.c_str(), std::ios_base::in);
  return CutModel::load(infile);
}
