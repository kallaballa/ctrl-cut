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
    zerolength(0)
  { }

  //deep copy
  CutModel(const CutModel& other) :
    settings(*static_cast<DocumentSettings* const>(other.settings.parent)), clipped(0), zerolength(0) {
    for(CutModel::const_iterator it = other.begin(); it != other.end(); it++) {
      this->add(* new Segment(**it));
    }
  }

  virtual ~CutModel() {
    this->clear();
  }

  iterator begin() { return this->segmentIndex.begin(); }
  const_iterator begin() const  { return this->segmentIndex.begin(); }
  iterator end() { return this->segmentIndex.end(); }
  const_iterator end() const  { return this->segmentIndex.end(); }
  SegmentList::reference front() { return this->segmentIndex.front(); }
  SegmentList::reference back() { return this->segmentIndex.back(); }
  size_t size() const { return this->segmentIndex.size(); }
  bool empty() const { return this->segmentIndex.empty(); }

  bool createSegment(const Point&  p1, const Point&  p2, const OpParams& settings);
  bool createSegment(const int32_t& inX,const int32_t& inY,const int32_t& outX,const int32_t& outY,const OpParams& settings);

  virtual void remove(Segment& seg);
  virtual iterator erase(iterator it);
  virtual void clear();

  bool wasClipped() const {
    return this->clipped > 0;
  }

  bool load(const std::string &filename);
  bool load(std::istream &input);
  void clip(Segment& seg);
protected:
  //FIXME should be managed by settings
  uint64_t clipped;
  uint64_t zerolength;
  SegmentList segmentIndex;
  virtual void add(Segment& seg);
};

void make_route(StringList& strings, CutModel& model);
#endif /* CUTMODEL_H_ */
