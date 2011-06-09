#include "CutModel.h"

#include <fstream>

void CutModel::createSegment(const Point &p1, const Point &p2,
    CutSettings& settings) {
  if (p1 == p2) // ignore zero length segments
    return;

  addSegment(*new Segment(p1, p2, settings));
}

Segment& CutModel::clipSegmentToLaserBed(Segment &unclipped) {
  Segment* seg = &unclipped;
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

  if(seg->first.x > this->config.device_width - 1 || seg->second.x > this->config.device_width - 1) {
    if(seg->first.x > this->config.device_width - 1 && seg->second.x > this->config.device_width - 1)
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

  if(seg->first.y > this->config.device_height - 1 || seg->second.y > this->config.device_height - 1) {
    if(seg->first.y > this->config.device_height - 1 && seg->second.y > this->config.device_height - 1)
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
void CutModel::createSegment(int32_t inX, int32_t inY, int32_t outX,
    int32_t outY, CutSettings& settings) {
  createSegment(*(new Point(inX, inY)), *(new Point(outX, outY)), settings);
}

void CutModel::addSegment(Segment& seg) {
  Segment& clipped = clipSegmentToLaserBed(seg);
  if (clipped.first == clipped.second) // ignore zero length segments
    return;

  CutModel::iterator it_clipped = segmentIndex.insert(end(), &clipped);
  std::pair<SpatialItem, SpatialItem>& items = createSpatialItems(it_clipped);
  spatialIndex.insert(items.first);
  spatialIndex.insert(items.second);
}

CutModel::iterator CutModel::removeSegment(iterator it_seg) {
  assert(it_seg != end());
  std::pair<SpatialItem, SpatialItem>& items = createSpatialItems(it_seg);
  spatialIndex.erase_exact(items.first);
  spatialIndex.erase_exact(items.second);
  return segmentIndex.erase(it_seg);
}

void CutModel::findWithinRange(iterator it_seg, std::vector<SpatialItem> v) {
  const Sphere& bsphere = (*it_seg)->getSphere();
  SpatialItem scenter = *new SpatialItem(it_seg, bsphere.center);
  spatialIndex.find_within_range(scenter, bsphere.radius, std::back_inserter(v));
}

CutGraph& CutModel::createCutGraph() const {
  CutGraph& graph = (*new CutGraph());

  for (CutModel::const_iterator it = begin(); it != end(); ++it)
    graph.createEdge(*(*it));

  return graph;
}

std::pair<SpatialItem, SpatialItem>& CutModel::createSpatialItems(
    iterator it_seg) {
  Segment& seg = *(*it_seg);
  SpatialItem si_first = (*new SpatialItem(it_seg, seg.first));
  SpatialItem si_second = (*new SpatialItem(it_seg, seg.second));
  return *new std::pair<SpatialItem, SpatialItem>(si_first, si_second);
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
  return model;
}

/*!
 Loads vector data from EPS/Ghostscript output from the given file
 */
CutModel *CutModel::load(const std::string &filename) {
  std::ifstream infile(filename.c_str(), std::ios_base::in);
  return CutModel::load(infile);
}
