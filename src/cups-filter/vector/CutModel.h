#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>
#include <list>
#include "Geometry.h"
#include "CutGraph.h"

class CutModel {
public:
  typedef SegmentList::iterator iterator;
  typedef SegmentList::const_iterator const_iterator;

  CutModel() :
    clipped(false), spatialIndex(std::ptr_fun(spatial_item_ac)) {
  }
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
  void swap(SegmentList &v) { this->segmentIndex.swap(v); }

  void createSegment(const Point &p1, const Point &p2, CutSettings& settings);
  void createSegment(int32_t inX, int32_t inY, int32_t outX, int32_t outY, CutSettings& settings);
  void addSegment(Segment& seg);
  iterator removeSegment(iterator it_seg);
  void findWithinRange(iterator it_seg, std::vector<SpatialItem> v);
  CutGraph& createCutGraph() const;

  static CutModel *load(const std::string &filename);
  static CutModel *load(std::istream &input);

  bool wasClipped() const {
    return this->clipped;
  }

private:
  bool clipped;
  SegmentList segmentIndex;
  SpatialTree spatialIndex;

  std::pair<SpatialItem, SpatialItem>& createSpatialItems(iterator it_seg);
};

inline std::ostream& operator<<(std::ostream &os, const CutModel::iterator it) {
//FIXME
  return os;
}

#endif /* CUTMODEL_H_ */
