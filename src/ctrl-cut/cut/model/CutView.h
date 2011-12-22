#ifndef CUTVIEW_H_
#define CUTVIEW_H_

#include <set>
#include "cut/geom/Geometry.h"
#include "CutModel.h"

class CutView : public CutModel {
public:
  CutView(CutModel model) : CutModel(model){}

  virtual ~CutView() {
    clear();
  }

  void add(const Segment& seg);
  void remove(const Segment& seg);
  iterator erase(iterator it);
  void clear();

private:
  typedef std::set<const Segment*> SegmentSet;
  SegmentSet increment;

};

#endif /* CUTVIEW_H_ */
