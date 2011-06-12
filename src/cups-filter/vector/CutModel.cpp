#include "CutModel.h"

#include <fstream>

void CutModel::createSegment(const Point &p1, const Point &p2,
    CutSettings& settings) {
  if (p1 == p2) // ignore zero length segments
    return;

  add(*new Segment(p1, p2, settings));
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

void CutModel::add(const Segment& seg) {
  const Segment& clipped = clipSegmentToLaserBed(seg);
  if (clipped.first == clipped.second) // ignore zero length segments
    return;

  CutModel::SegmentIter it_clipped = segmentIndex.insert(endSegments(), &clipped);
  std::pair<SegmentNode, SegmentNode>& items = createSegmentNodes(it_clipped);
  segmentTree.insert(items.first);
  segmentTree.insert(items.second);
}

CutModel::SegmentIter CutModel::remove(SegmentIter it_seg) {
  assert(it_seg != endSegments());
  std::pair<SegmentNode, SegmentNode>& items = createSegmentNodes(it_seg);
  segmentTree.erase_exact(items.first);
  segmentTree.erase_exact(items.second);
  return segmentIndex.erase(it_seg);
}

void CutModel::add(const SegmentString& string) {
  CutModel::StringIter it_str = stringIndex.insert(endStrings(), &string);
/*  std::vector<StringNode>& nodes = createStringNodes(it_str);
  for(std::vector<StringNode>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n)
    stringTree.insert(*it_n);*/
}

CutModel::StringIter CutModel::remove(StringIter it_str) {
  assert(it_str != endStrings());

  /*std::vector<StringNode>& nodes = createStringNodes(it_str);
  for(std::vector<StringNode>::iterator it_n = nodes.begin(); it_n != nodes.end(); ++it_n) {
    std::cerr << (*it_n).point << std::endl;
    stringTree.erase_exact(*it_n);
  }*/
  return stringIndex.erase(it_str);
}

void CutModel::findWithinRange(SegmentIter it_seg, std::vector<SegmentNode> v) {
  const Sphere& bsphere = (*it_seg)->getSphere();
  SegmentNode scenter = *new SegmentNode(it_seg, bsphere.center);
  segmentTree.find_within_range(scenter, bsphere.radius, std::back_inserter(v));
}

SegmentTree::const_iterator CutModel::findSegment(const Point& p) {
  return segmentTree.find(*new SegmentNode(segmentIndex.end(), p));
}

StringTree::const_iterator CutModel::findString(const Point& p) {
  return stringTree.find(*new StringNode(stringIndex.end(), p));
}

std::pair<SegmentNode, SegmentNode>& CutModel::createSegmentNodes(
    SegmentIter it_seg) {
  const Segment& seg = *(*it_seg);
  SegmentNode si_first = (*new SegmentNode(it_seg, seg.first));
  SegmentNode si_second = (*new SegmentNode(it_seg, seg.second));
  return *new std::pair<SegmentNode, SegmentNode>(si_first, si_second);
}

void CutModel::remove(const SegmentString& string) {
  StringIter it = find(stringIndex.begin(), stringIndex.end(), &string);
  if(it != stringIndex.end())
    remove(it);
}

std::vector<StringNode>& CutModel::createStringNodes(StringIter it_str){
  std::vector<StringNode>& nodes = * new std::vector<StringNode>();
  const SegmentString& segStr = *(*it_str);
  SegmentString::PointConstIter it = segStr.beginPoints();
  for(; it != segStr.endPoints(); ++it) {
    nodes.push_back(* new StringNode(it_str, *(*it)));
  }

  return nodes;
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

  if (model->segmentsEmpty()) {
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
