#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>
#include <list>
#include "cut/geom/Geometry.h"
#include "cut/geom/SegmentTree.h"
#include "config/CutSettings.h"
#include "config/DocumentSettings.h"

class CutModel {
public:
  CutSettings settings;
  static DocumentSettings defaultDocSettings;
  typedef SegmentList::iterator iterator;
  typedef SegmentList::const_iterator const_iterator;

  CutModel(DocumentSettings& docSettings = defaultDocSettings) :
    settings(docSettings),
    clipped(0),
    zerolength(0),
    segmentTree()
  { }

  virtual ~CutModel() {}

  iterator begin() { return this->segmentIndex.begin(); }
  const_iterator begin() const  { return this->segmentIndex.begin(); }
  iterator end() { return this->segmentIndex.end(); }
  const_iterator end() const  { return this->segmentIndex.end(); }
  SegmentList::reference front() { return this->segmentIndex.front(); }
  SegmentList::reference back() { return this->segmentIndex.back(); }
  size_t size() const { return this->segmentIndex.size(); }
  bool empty() const { return this->segmentIndex.empty(); }
  bool createSegment(const Point &p1, const Point &p2, OpParams& settings);
  bool createSegment(int32_t inX, int32_t inY, int32_t outX, int32_t outY, OpParams& settings);

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
    this->settings = other.settings;
    this->segmentIndex = other.segmentIndex;
    this->segmentTree = other.segmentTree;
  }

  bool load(const std::string &filename);
  bool load(std::istream &input);
protected:
  uint64_t clipped;
  uint64_t zerolength;
  SegmentTree segmentTree;
  SegmentList segmentIndex;

private:
  const Segment& clipSegmentToLaserBed(const Segment& seg);
};

void make_route(StringList& strings, CutModel& model);
#endif /* CUTMODEL_H_ */
