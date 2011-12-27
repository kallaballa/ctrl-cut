#if 0
#define CUTVIEW_H_

#include <set>
#include "cut/geom/Geometry.h"
#include "CutModel.h"


template<typename Tbase>
struct compound_list_iterator : public Tbase {
  typedef compound_list_iterator _Self;

  typedef ptrdiff_t difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef Segment* value_type;
  typedef Segment** pointer;
  typedef Segment*& reference;

  Tbase begin1;
  Tbase begin2;
  Tbase end1;
  Tbase end2;
  Tbase targetIter;

  bool first;

  compound_list_iterator() :
    begin1(), begin2(), end1(), end2(), targetIter(), first(true) {
  }

  explicit compound_list_iterator(Tbase begin1, Tbase begin2, Tbase end1, Tbase end2, Tbase targetIter, bool first) :
        begin1(begin1), begin2(begin2), end1(end1), end2(end2), targetIter(targetIter), first(first) {
  }

  reference operator*() const {
    return *targetIter;
  }

  pointer operator->() const {
    return targetIter;
  }

  _Self&
  operator++() {
    if(first && targetIter == end1){
      targetIter = begin2;
      first = false;
    }

    targetIter++;
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    (*this)++;
    return __tmp;
  }

  _Self&
  operator--() {
    if(!first && targetIter == begin2){
      targetIter = end1;
    }

    targetIter--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    (*this)--;
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
    return targetIter == __x.targetIter;
  }

  bool operator!=(const _Self& __x) const {
    return targetIter != __x.targetIter;
  }
};

class CutView : public CutModel {
public:
  typedef compound_list_iterator<SegmentList::iterator> iterator;
  CutView(CutModel model) : CutModel(model){}

  virtual ~CutView() {
    clear();
  }

  void remove(Segment& seg);
  iterator erase(iterator it);

  iterator begin() { return iterator(segmentIndex.begin(), segmentIndex.end(), increment.begin(), increment.end(), segmentIndex.begin(), true); }
  iterator end() { return iterator(increment.end(), increment.end(), increment.end(), increment.end(), increment.end(), false); }

  SegmentList::reference front() { return this->segmentIndex.front(); }
  SegmentList::reference back() { return this->increment.back(); }

  size_t size() const { return this->segmentIndex.size() + this->increment.size(); }
  bool empty() const { return this->segmentIndex.empty() && this->increment.empty(); }

  void clear();

private:
  SegmentList increment;
protected:
  virtual void add(Segment& seg);
};

#endif /* CUTVIEW_H_ */
