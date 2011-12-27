#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>
#include <list>

#include "config/CutSettings.h"
#include "cut/geom/Segment.h"

class Document;
class CutModel {
public:
  typedef std::list<Segment> SegmentList;
  typedef SegmentList::iterator iterator;
  typedef SegmentList::const_iterator const_iterator;
  typedef SegmentList::reference reference;
  typedef SegmentList::const_reference const_reference;

  CutModel(Document& parent);

  //shallow copy
  CutModel(const CutModel& other) : parent(other.getParent()), settings(other.getSettings()), clipped(0), zerolength(0) {
  }

  ~CutModel() {
    this->clear();
  }

  CutSettings& getSettings() {
    return settings;
  }

  const CutSettings getSettings() const {
    return settings;
  }

  virtual iterator begin() { return this->segmentIndex.begin(); }
  virtual const_iterator begin() const  { return this->segmentIndex.begin(); }
  virtual iterator end() { return this->segmentIndex.end(); }
  virtual const_iterator end() const  { return this->segmentIndex.end(); }
  virtual reference front() { return this->segmentIndex.front(); }
  virtual reference back() { return this->segmentIndex.back(); }
  virtual const_reference front() const { return this->segmentIndex.front(); }
  virtual const_reference back() const { return this->segmentIndex.back(); }
  virtual size_t size() const { return this->segmentIndex.size(); }
  virtual bool empty() const { return this->segmentIndex.empty(); }

  virtual void push_front(const Segment& seg);
  virtual void push_back(const Segment& seg);
  virtual void splice(iterator pos, CutModel& other, iterator begin, iterator end);
  virtual iterator find(const Segment& seg) { return std::find(begin(),end(), seg); };
  virtual void remove(Segment& seg);
  virtual iterator erase(iterator it);
  virtual void clear();
  virtual void copy(const CutModel& other);

  bool create(const Segment& segment);
  bool create(const Point&  p1, const Point&  p2);
  bool create(const Point&  p1, const Point&  p2, const Segment& seg);
  bool create(const int32_t& inX,const int32_t& inY,const int32_t& outX,const int32_t& outY);

  bool wasClipped() const {
    return this->clipped > 0;
  }

  bool load(const std::string &filename);
  bool load(std::istream &input);
  void clip(Segment& seg);
  void operator=(const CutModel& other);

  bool hasParent() const {
    return parent != NULL;
  }

  Document* getParent() const {
    return parent;
  }

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    settings.put(key,value);
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) const {
    return settings.get(key);
  }
protected:
  //FIXME should be managed by settings
  Document* parent;
  CutSettings settings;
  uint64_t clipped;
  uint64_t zerolength;
  SegmentList segmentIndex;
};


#endif /* CUTMODEL_H_ */
