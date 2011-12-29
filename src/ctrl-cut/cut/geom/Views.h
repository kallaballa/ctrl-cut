#ifndef VIEWS_H_
#define VIEWS_H_

#include <list>
#include "Geometry.h"
#include "Segment.h"

template<
  typename Tparent,
  typename Tbase,
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct MultiIter : public Tcontainer<Tbase, Tallocator<Tbase> >::iterator {
  typedef Tparent _Parent;
  typedef MultiIter<Tparent,Tbase,Tcontainer,Tallocator> _Self;

  typedef typename _Parent::iterator _BaseIter;
  typedef typename Tcontainer<Tparent, Tallocator<Tparent> >::iterator _MultiIter;

  typedef typename _BaseIter::difference_type difference_type;;
  typedef typename _BaseIter::iterator_category iterator_category;
  typedef typename _BaseIter::value_type value_type;
  typedef typename _BaseIter::pointer pointer;
  typedef typename _BaseIter::reference reference;

  _MultiIter parentBegin;
  _MultiIter parentEnd;
  _MultiIter parentCurrent;
  _BaseIter current;

  explicit MultiIter(_MultiIter begin, _MultiIter end) :
    parentBegin(begin), parentEnd(end), current(getCurrentBegin()) {}

  MultiIter(const _MultiIter& other) :
    parentBegin(other.parentBegin), parentEnd(other.parentEnd), current(other.current) {}

  typename _Self::reference operator*() const {
    return *current;
  }

  typename _Self::pointer operator->() const {
    return &*current;
  }

  void nextParent() {
    parentCurrent++;
    current = getCurrentBegin();
  }

  void prevParent() {
    parentCurrent--;
    current = getCurrentEnd();
  }

  _Self&
  operator++() {
    if(current == getCurrentEnd())
      nextParent();

    current++;
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  _Self&
  operator--() {
    if(current == getCurrentBegin())
      prevParent();

    current--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    --(*this);
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
    return current == __x.current;
  }

  bool operator!=(const _Self& __x) const {
    return current != __x.current;
  }

  _BaseIter getCurrentBegin() {
    return (*parentCurrent).begin();
  }

  _BaseIter getCurrentEnd() {
    return (*parentCurrent).end();
  }
};

template<
  typename Tparent,
  typename Tbase,
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct MultiConstIter : public Tcontainer<Tbase, Tallocator<Tbase> >::const_iterator {
  typedef Tparent _Parent;
  typedef MultiConstIter<Tparent,Tbase,Tcontainer,Tallocator> _Self;

  typedef typename Tparent::const_iterator _BaseIter;
  typedef typename Tcontainer<Tparent, Tallocator<Tparent> >::const_iterator _MultiIter;

  typedef typename _BaseIter::difference_type difference_type;;
  typedef typename _BaseIter::iterator_category iterator_category;
  typedef typename _BaseIter::value_type value_type;
  typedef typename _BaseIter::pointer pointer;
  typedef typename _BaseIter::reference reference;

  _MultiIter parentBegin;
  _MultiIter parentEnd;
  _MultiIter parentCurrent;
  _BaseIter current;

  explicit MultiConstIter(_MultiIter begin, _MultiIter end) :
    parentBegin(begin), parentEnd(end), current(getCurrentBegin()) {}

  MultiConstIter(const _Self& other) :
    parentBegin(other.parentBegin), parentEnd(other.parentEnd), current(other.current) {}

  typename _Self::reference operator*() const {
    return *current;
  }

  typename _Self::pointer operator->() const {
    return &*current;
  }

  void nextParent() {
    parentCurrent++;
    current = getCurrentBegin();
  }

  void prevParent() {
    parentCurrent--;
    current = getCurrentEnd();
  }

  _Self&
  operator++() {
    if(current == getCurrentEnd())
      nextParent();

    current++;
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  _Self&
  operator--() {
    if(current == getCurrentBegin())
      prevParent();

    current--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    --(*this);
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
    return current == __x.current;
  }

  bool operator!=(const _Self& __x) const {
    return current != __x.current;
  }

  _BaseIter getCurrentBegin() {
    return (*parentCurrent).begin();
  }

  _BaseIter getCurrentEnd() {
    return (*parentCurrent).end();
  }
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct SegmentWise {
  typedef typename Tcontainer<Point, Tallocator<Point> >::const_iterator _ParentIter;
  typedef SegmentWise<Tcontainer, Tallocator> _Self;

  _ParentIter begin;
  _ParentIter end;
  _ParentIter last;
  _ParentIter current;

  explicit SegmentWise(_ParentIter parentBegin, _ParentIter parentEnd) :
    begin(parentBegin), end(parentEnd), last(parentBegin), current(++parentBegin){
    // check we are having either no or at least 2 points
    assert(begin != end && current == end);
  }
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct SegmentWiseIterator :
  public Tcontainer<Segment, Tallocator<Segment> >::iterator,
  public SegmentWise<Tcontainer,Tallocator> {
    typedef SegmentWiseIterator<Tcontainer,Tallocator> _Self;
    typedef SegmentWise<Tcontainer,Tallocator> _Wise;
    typedef typename Tcontainer<Point, Tallocator<Point> >::iterator _ParentIter;
    typedef typename Tcontainer<Segment, Tallocator<Segment> >::iterator _ChildIter;

  explicit SegmentWiseIterator(_ParentIter parentBegin, _ParentIter  parentEnd) :
    SegmentWise<Tcontainer,Tallocator> (parentBegin, parentEnd)
     {}

  typename _ChildIter::reference operator*() const {
    //FIXME
    return * new Segment(*_Wise::last, *_Wise::current);
  }

  typename _ChildIter::pointer operator->() const {
    return &Segment(*_Wise::last, *_Wise::current);
  }

  _Self&
  operator++() {
    _Wise::last = _Wise::current++;
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  _Self&
  operator--() {
    _Wise::last = _Wise::current--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

//FIXME concept check
  virtual bool operator==(const _Self& __x) const {
    return _Wise::current == __x.current;
  }

  virtual bool operator!=(const _Self& __x) const {
    return _Wise::current != __x.current;
  }
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct SegmentWiseConstIterator :
  public Tcontainer<Segment, Tallocator<Segment> >::const_iterator,
  public SegmentWise<Tcontainer,Tallocator> {
    typedef SegmentWiseConstIterator<Tcontainer,Tallocator> _Self;
    typedef SegmentWise<Tcontainer,Tallocator> _Wise;
    typedef typename Tcontainer<Point, Tallocator<Point> >::const_iterator _ParentIter;
    typedef typename Tcontainer<Segment, Tallocator<Segment> >::const_iterator _ChildIter;

  explicit SegmentWiseConstIterator(_ParentIter parentBegin, _ParentIter  parentEnd) :
    SegmentWise<Tcontainer,Tallocator> (parentBegin, parentEnd)
     {}

  typename _ChildIter::reference operator*() const {
    //FIXME
    return * new Segment(*_Wise::last, *_Wise::current);
  }

  typename _ChildIter::pointer const operator->() const {
    return &Segment(*_Wise::last, *_Wise::current);
  }

  _Self&
  operator++() {
    _Wise::last = _Wise::current++;
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  _Self&
  operator--() {
    _Wise::last = _Wise::current--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

//FIXME concept check
  virtual bool operator==(const _Self& __x) const {
    return _Wise::current == __x.current;
  }

  virtual bool operator!=(const _Self& __x) const {
    return _Wise::current != __x.current;
  }
};

template<
  typename Titer,
  typename TconstIter,
  typename Tcontainer
>
class View {
  Tcontainer* container;
public:
  typedef Titer iterator;
  typedef TconstIter const_iterator;

  View(Tcontainer& container) : container(&container) {}
  Titer begin() {
    return Titer(container->begin(), container->end());
  }
  TconstIter begin() const {
    return TconstIter(container->begin(), container->end());
  }
  Titer end()  {
    return Titer(container->end(), container->end());
  }
  TconstIter end() const {
    return TconstIter(container->end(), container->end());
  }
  typename Titer::reference front() {
    return *Titer(container->begin(), container->begin()+1);
  }
  typename Titer::reference back() {
    return *Titer(container->end()-1, container->end());
  }
  typename TconstIter::reference front() const {
    return *TconstIter(container->begin(), container->begin()+1);
  }
  typename TconstIter::reference back() const {
    return *TconstIter(container->end()-1, container->end());
  }
  bool empty() const { //FIXME
    return container->empty() || this->size() == 0;
  }

  size_t size() const {  //FIXME
    return std::distance(this->begin(), this->end());
  }
};

#endif /* VIEWS_H_ */
