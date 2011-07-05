#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>
#include <list>
#include "util/LaserConfig.h"
#include "vector/geom/Geometry.h"
#include "vector/geom/SegmentTree.h"

class CutModel {
public:
  typedef SegmentList::iterator iterator;
  typedef SegmentList::const_iterator const_iterator;

  CutModel() :
    config(LaserConfig::inst()),
    clipped(false),
    segmentTree(),
    leftBedBorder(*new Point(0, 0), *new Point(0, config.device_height-1), *new CutSettings(0,0,0)),
    bottomBedBorder(*new Point(0, config.device_height-1), *new Point(config.device_width-1,config.device_height-1), *new CutSettings(0,0,0)),
    rightBedBorder(*new Point(config.device_width-1, config.device_height-1), *new Point(config.device_width-1, 0), *new CutSettings(0,0,0)),
    topBedBorder(*new Point(config.device_width-1, 0), *new Point(0, 0), *new CutSettings(0,0,0)),
    translation(std::make_pair(0,0))
  {}

  virtual ~CutModel() {
  }

  iterator begin() { return this->segmentIndex.begin(); }
  const_iterator begin() const  { return this->segmentIndex.begin(); }
  iterator end() { return this->segmentIndex.end(); }
  const_iterator end() const  { return this->segmentIndex.end(); }
  SegmentList::reference front() { return this->segmentIndex.front(); }
  SegmentList::reference back() { return this->segmentIndex.back(); }
  size_t size() const { return this->segmentIndex.size(); }
  bool empty() const { return this->segmentIndex.empty(); }
  const std::pair<int32_t, int32_t>& getTranslation() { return translation; }
  bool createSegment(const Point &p1, const Point &p2, CutSettings& settings);
  bool createSegment(int32_t inX, int32_t inY, int32_t outX, int32_t outY, CutSettings& settings);

  void add(const Segment& seg);
  iterator remove(iterator it_seg);
  void remove(const Segment& seg);

  bool wasClipped() const {
    return this->clipped;
  }

  const SegmentTree& getSegmentTree() {
    return segmentTree;
  }

  void operator=(CutModel& other) {
    this->config = other.config;
    this->clipped = other.clipped;
    this->segmentIndex = other.segmentIndex;
    this->segmentTree = other.segmentTree;
  }

  static CutModel *load(const std::string &filename);
  static CutModel *load(std::istream &input);
  LaserConfig& config;
protected:
  bool clipped;
  SegmentTree segmentTree;
  SegmentList segmentIndex;

private:
  Segment leftBedBorder;
  Segment bottomBedBorder;
  Segment rightBedBorder;
  Segment topBedBorder;
  std::pair<int32_t, int32_t> translation;

  const Segment& clipSegmentToLaserBed(const Segment& seg);
};

void make_route(StringList& strings, CutModel& model);
#endif /* CUTMODEL_H_ */
