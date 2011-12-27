#ifndef ROUTE_H_
#define ROUTE_H_

#include <algorithm>
#include "Geometry.h"
#include "cut/geom/SegmentString.h"

class CutModel;

template<typename T>
struct pointer_list_iterator : public std::list<T>::iterator {
  typedef typename std::list<T*> PointerList;
  typedef typename PointerList::iterator PointerListIt;
  typedef pointer_list_iterator _Self;
  typedef ptrdiff_t difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  const PointerList& pointerList;
  PointerListIt currentListIt;
  bool isEnd;

  pointer_list_iterator() : pointerList(PointerList()), isEnd(true) {}

  explicit pointer_list_iterator(PointerList& pointerList) :
    pointerList(pointerList), currentListIt(pointerList.begin()), isEnd(pointerList.empty()) {}

  typename std::list<T>::iterator::reference operator*() const {
    return **currentListIt;
  }

  typename std::list<T>::iterator::pointer operator->() const {
    return *currentListIt;
  }

  _Self&
  operator++() {
    currentListIt++;
    if(currentListIt == pointerList.end()) {
      isEnd=true;
    }
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    currentListIt++;
    if(currentListIt == pointerList.end()) {
      isEnd=true;
    }
    return __tmp;
  }

  _Self&
  operator--() {
    currentListIt--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    currentListIt--;
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
    return (isEnd==true && __x.isEnd== true) || currentListIt == __x.currentListIt;
  }

  bool operator!=(const _Self& __x) const {
    return !(isEnd == true && __x.isEnd == true) && currentListIt != __x.currentListIt;
  }
};

template<typename T>
struct const_pointer_list_iterator : public std::list<T>::iterator {
  typedef typename std::list<T*> PointerList;
  typedef typename PointerList::const_iterator PointerListIt;
  typedef const_pointer_list_iterator _Self;
  typedef ptrdiff_t difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  const PointerList& pointerList;
  const PointerListIt currentListIt;
  bool isEnd;

  const_pointer_list_iterator() : pointerList(PointerList()), isEnd(true) {}

  explicit const_pointer_list_iterator(const PointerList& pointerList) :
    pointerList(pointerList), currentListIt(pointerList.begin()), isEnd(pointerList.empty()) {}

  typename std::list<T>::iterator::reference operator*() const {
    return **currentListIt;
  }

  typename std::list<T>::iterator::pointer operator->() const {
    return *currentListIt;
  }

  _Self&
  operator++() {
    currentListIt++;
    if(currentListIt == pointerList.end()) {
      isEnd=true;
    }
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    (*this)++;
    return __tmp;
  }

  _Self&
  operator--() {
    currentListIt--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    (*this)--;
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
    return currentListIt == __x.currentListIt;
  }

  bool operator!=(const _Self& __x) const {
    return currentListIt != __x.currentListIt;
  }
};
template<typename TParent, typename T>
struct compound_iterator : public pointer_list_iterator<T> {
  typedef compound_iterator _Self;
  typedef ptrdiff_t difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  typedef typename std::list<T>::iterator TargetIt;
  typedef pointer_list_iterator<TParent> ParentIt;

  ParentIt parentIter;
  TargetIt targetIter;

  compound_iterator() {
  }

  explicit compound_iterator(pointer_list_iterator<TParent> parentIter) :
    parentIter(parentIter) {
  }

  typename std::list<T>::reference operator*() const {
    return *targetIter;
  }

  typename std::list<T>::pointer operator->() const {
    return targetIter;
  }

  void nextParent() {
    parentIter++;
    targetIter = getCurrentBegin();
  }

  void prevParent() {
    parentIter--;
    targetIter = getCurrentEnd();
  }

  _Self&
  operator++() {
    if(targetIter == getCurrentEnd()){
      nextParent();
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
    if(targetIter == getCurrentBegin()){
      prevParent();
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

  TargetIt getCurrentIter() {
    return targetIter;
  }

  TargetIt getCurrentBegin() {
    return (*parentIter).begin();
  }

  TargetIt getCurrentEnd() {
    return (*parentIter).end();
  }
};

template<typename TParent, typename T>
struct const_compound_iterator : public const_pointer_list_iterator<T> {
  typedef const_compound_iterator _Self;
  typedef ptrdiff_t difference_type;
  typedef std::bidirectional_iterator_tag iterator_category;

  typedef typename std::list<T>::iterator TargetIt;
  typedef const_pointer_list_iterator<TParent> ParentIt;

  ParentIt parentIter;
  TargetIt targetIter;

  const_compound_iterator() {
  }

  explicit const_compound_iterator(const_pointer_list_iterator<TParent> parentIter) :
    parentIter(parentIter) {
  }

  typename std::list<T>::reference operator*() const {
    return *targetIter;
  }

  typename std::list<T>::pointer operator->() const {
    return targetIter;
  }

  void nextParent() {
    parentIter++;
    targetIter = getCurrentBegin();
  }

  void prevParent() {
    parentIter--;
    targetIter = getCurrentEnd();
  }

  _Self&
  operator++() {
    if(targetIter == getCurrentEnd()){
      nextParent();
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
    if(targetIter == getCurrentBegin()){
      prevParent();
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

  TargetIt getCurrentIter() {
    return targetIter;
  }

  TargetIt getCurrentBegin() {
    return (*parentIter).begin();
  }

  TargetIt getCurrentEnd() {
    return (*parentIter).end();
  }
};

class Route : public CutModel {
public:
  typedef compound_iterator<SegmentString,Segment> iterator;
  typedef const_compound_iterator<SegmentString,Segment> const_iterator;
  typedef std::list<SegmentString* > StringList;
  typedef pointer_list_iterator<SegmentString> StringIter;
  typedef const_pointer_list_iterator<SegmentString> StringConstIter;

  Route(const CutModel& model);

  StringIter beginStrings() { return StringIter(this->strings); }
  StringIter endStrings() { return StringIter(); }
  StringConstIter beginStrings() const { return StringConstIter(this->strings); }
  StringConstIter endStrings() const { return StringConstIter(); }
  StringIter::reference frontStrings()  { return *this->strings.front(); }
  StringIter::reference backStrings()  { return *this->strings.back(); }
  size_t numStrings() const { return this->strings.size(); }
  bool emptyStrings() const { return this->strings.empty(); }
  void splice(iterator pos, iterator begin, iterator end) {
    assert(!"not implemented");
  }
  bool append(const Segment& seg);
  CutModel::iterator begin();
  CutModel::const_iterator begin() const;
  void push_front(const Segment& seg);
  void push_back(const Segment& seg);
  void remove(Segment& seg);
  CutModel::iterator erase(iterator it);
  void clear();
  void copy(const Route& other);
  bool empty() const;

  CutModel::iterator end() { return this->strings.back()->end(); }
  CutModel::const_iterator end() const  { return this->strings.back()->end(); }
  CutModel::reference front() { return this->strings.front()->front(); }
  CutModel::reference back() { return this->strings.back()->back(); }
  CutModel::const_reference front() const { return this->strings.front()->front(); }
  CutModel::const_reference back() const { return this->strings.back()->back(); }

  friend std::ostream& operator<<(std::ostream &os, Route& route)  {
    os << "<route>" << std::endl;
    for(Route::StringIter it=route.beginStrings(); it != route.endStrings(); ++it)
      os << (*it) << std::endl;
    os << std::endl;
    os << "</route>" << std::endl;
    return os;
  }
  static Route& make(CutModel& model);
private:


  StringList strings;
};

#endif
