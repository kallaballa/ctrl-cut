#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>
#include <list>
#include "vector/geom/Geometry.h"
#include "vector/geom/SegmentTree.h"
#include "config/LaserConfig.h"

class CutModel {
public:
  typedef SegmentList::iterator iterator;
  typedef SegmentList::const_iterator const_iterator;

  CutModel(LaserConfig& config) :
    config(config),
    clipped(0),
    zerolength(0),
    segmentTree(),
    leftBedBorder(*new Point(0, 0), *new Point(0, config.getGraphicsDeviceHeight()-1), *new CutSettings(0,0,0)),
    bottomBedBorder(*new Point(0, config.getGraphicsDeviceHeight()-1), *new Point(config.getGraphicsDeviceWidth()-1,config.getGraphicsDeviceHeight()-1), *new CutSettings(0,0,0)),
    rightBedBorder(*new Point(config.getGraphicsDeviceWidth()-1, config.getGraphicsDeviceHeight()-1), *new Point(config.getGraphicsDeviceWidth()-1, 0), *new CutSettings(0,0,0)),
    topBedBorder(*new Point(config.getGraphicsDeviceWidth()-1, 0), *new Point(0, 0), *new CutSettings(0,0,0)),
    translation(Point(0,0))
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
  void setTranslation(const Point p) { this->translation = p; }
  const Point& getTranslation() { return translation; }
  bool createSegment(const Point &p1, const Point &p2, CutSettings& settings);
  bool createSegment(int32_t inX, int32_t inY, int32_t outX, int32_t outY, CutSettings& settings);

  void add(const Segment& seg);
  iterator remove(iterator it_seg);
  void remove(const Segment& seg);

  bool wasClipped() const {
    return this->clipped > 0;
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

  static CutModel *load(LaserConfig& config, const std::string &filename);
  static CutModel *load(LaserConfig& config, std::istream &input);

  LaserConfig& config;
protected:
  uint64_t clipped;
  uint64_t zerolength;
  SegmentTree segmentTree;
  SegmentList segmentIndex;

private:

  Segment leftBedBorder;
  Segment bottomBedBorder;
  Segment rightBedBorder;
  Segment topBedBorder;
  Point translation;

  const Segment& clipSegmentToLaserBed(const Segment& seg);
};

void make_route(StringList& strings, CutModel& model);
#endif /* CUTMODEL_H_ */
