#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>
#include <list>
#include "util/LaserConfig.h"
#include "Geometry.h"
#include "CutGraph.h"

class CutModel {
public:
  typedef SegmentList::iterator SegmentIter;
  typedef SegmentList::const_iterator SegmentConstIter;
  typedef StringList::iterator StringIter;
  typedef StringList::const_iterator StringConstIter;

  CutModel() :
    config(LaserConfig::inst()),
    clipped(false),
    segmentTree(std::ptr_fun(segment_node_ac)),
    stringTree(std::ptr_fun(string_node_ac)),
    leftBedBorder(*new Point(0, 0), *new Point(0, config.device_height-1), *new CutSettings(0,0,0)),
    bottomBedBorder(*new Point(0, config.device_height-1), *new Point(config.device_width-1,config.device_height-1), *new CutSettings(0,0,0)),
    rightBedBorder(*new Point(config.device_width-1, config.device_height-1), *new Point(config.device_width-1, 0), *new CutSettings(0,0,0)),
    topBedBorder(*new Point(config.device_width-1, 0), *new Point(0, 0), *new CutSettings(0,0,0))
  {}

  virtual ~CutModel() {
  }

  SegmentIter beginSegments() { return this->segmentIndex.begin(); }
  SegmentConstIter beginSegments() const  { return this->segmentIndex.begin(); }
  SegmentIter endSegments() { return this->segmentIndex.end(); }
  SegmentConstIter endSegments() const  { return this->segmentIndex.end(); }
  SegmentList::reference frontSegments() { return this->segmentIndex.front(); }
  SegmentList::reference backSegments() { return this->segmentIndex.back(); }
  size_t numSegments() const { return this->segmentIndex.size(); }
  bool segmentsEmpty() const { return this->segmentIndex.empty(); }

  void clearSegments() {
    this->segmentIndex.clear();
    this->segmentTree.clear();
  }

  StringIter beginStrings() { return this->stringIndex.begin(); }
  StringConstIter beginStrings() const  { return this->stringIndex.begin(); }
  StringIter endStrings() { return this->stringIndex.end(); }
  StringConstIter endStrings() const  { return this->stringIndex.end(); }
  StringList::reference frontStrings() { return this->stringIndex.front(); }
  StringList::reference backStrings() { return this->stringIndex.back(); }
  size_t numStrings() const { return this->stringIndex.size(); }
  bool stringsEmpty() const { return this->stringIndex.empty(); }
  void clearStrings() {
    this->stringIndex.clear();
    this->stringTree.clear();
  }

  void createSegment(const Point &p1, const Point &p2, CutSettings& settings);
  void createSegment(int32_t inX, int32_t inY, int32_t outX, int32_t outY, CutSettings& settings);

  void add(const Segment& seg);
  SegmentIter remove(SegmentIter it_seg);
  void findWithinRange(SegmentIter it_seg, std::vector<SegmentNode> v);
  SegmentTree::const_iterator findSegment(const Point& p);

  void add(const SegmentString& seg);
  StringIter remove(StringIter it_seg);
  void remove(const SegmentString& string);
  StringTree::const_iterator findString(const Point& p);

  static CutModel *load(const std::string &filename);
  static CutModel *load(std::istream &input);

  bool wasClipped() const {
    return this->clipped;
  }

private:
  LaserConfig& config;
  bool clipped;
  SegmentList segmentIndex;
  SegmentTree segmentTree;

  StringList stringIndex;
  StringTree stringTree;

  Segment leftBedBorder;
  Segment bottomBedBorder;
  Segment rightBedBorder;
  Segment topBedBorder;

  const Segment& clipSegmentToLaserBed(const Segment& seg);
  std::pair<SegmentNode, SegmentNode>& createSegmentNodes(SegmentIter it_seg);
  std::vector<StringNode>& createStringNodes(StringIter it_str);
};

inline std::ostream& operator<<(std::ostream &os, const CutModel::SegmentIter it) {
  os << *(*it);
  return os;
}

inline std::ostream& operator<<(std::ostream &os, const CutModel::StringIter it) {
  return os;
}
#endif /* CUTMODEL_H_ */
