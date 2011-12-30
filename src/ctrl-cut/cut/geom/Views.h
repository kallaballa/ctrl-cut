#ifndef VIEWS_H_
#define VIEWS_H_

#include <list>
#include "Geometry.h"
#include "Segment.h"
#include "boost/function_output_iterator.hpp"

template <class X>
struct SegmentInputIterator
  : boost::InputIteratorConcept<X>
{
 private:
    typedef std::iterator_traits<X> t;
 public:
    typedef typename t::value_type value_type;
    BOOST_CONCEPT_ASSERT((boost::Convertible<value_type, Segment>));
};

template <class x>
struct SegmentOutputIterator
  : boost::OutputIteratorConcept<x,Segment>
{

};

template <class Tgeom>
struct SegmentAppender
{
  Tgeom* geom;

  SegmentAppender()
      : geom(NULL) {}
  SegmentAppender(Tgeom& geom)
      : geom(&geom) {}

  void operator()(const Segment& seg) const {
    geom->append(seg);
  }
};

template <typename Tgeom>
class SegmentAppendIterator : public boost::function_output_iterator<SegmentAppender<Tgeom> > {
public:
  typedef boost::function_output_iterator<SegmentAppender<Tgeom> > _Base;
  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  explicit SegmentAppendIterator() :
      boost::function_output_iterator<SegmentAppender<Tgeom> >(),
      geom(NULL) {};

  SegmentAppendIterator(Tgeom& geom) :
      boost::function_output_iterator<SegmentAppender<Tgeom> >(SegmentAppender<Tgeom>(geom)),
      geom(&geom)
      {};

  SegmentAppendIterator(const SegmentAppendIterator& other) :
      boost::function_output_iterator<SegmentAppender<Tgeom> >(SegmentAppender<Tgeom>(*other.geom)),
      geom(other.geom)
      {};

  void operator+=(const Segment& seg) {
      geom->push_back(seg);
  }
private:
  Tgeom* geom;
};

template<
  typename Tparent,
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct MultiIterBase {
  typedef Tparent _Parent;
  typedef MultiIterBase<Tparent,Tcontainer,Tallocator> _Self;
  typedef typename _Parent::iterator _ValueIter;
  typedef typename Tcontainer<Tparent, Tallocator<Tparent> >::iterator _SuperIter;

  _SuperIter parentBegin;
  _SuperIter parentEnd;
  _SuperIter parentCurrent;
  _ValueIter current;
  bool isEnd;

  explicit MultiIterBase(_SuperIter begin, _SuperIter end) :
    parentBegin(begin), parentEnd(end), parentCurrent(begin), isEnd(begin == end) {
    if(!isEnd)
      current = getCurrentBegin();
  }

  MultiIterBase(const _SuperIter& other) :
    parentBegin(other.parentBegin), parentEnd(other.parentEnd), current(other.current), isEnd(other.isEnd){}

  typename _ValueIter::reference operator*() const {
    return *current;
  }

  typename _ValueIter::pointer operator->() const {
    return &*current;
  }

  void nextParent() {
    if(!isEnd) {
      if(++parentCurrent == parentEnd) {
        isEnd = true;
      } else {
        current = getCurrentBegin();
      }
    }
  }

  void prevParent() {
    parentCurrent--;
    current = getCurrentEnd();
  }

  _Self&
  operator++() {
    current++;
    if(current == getCurrentEnd())
      nextParent();
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  _Self&
  operator--() {
    current--;
    if(current == getCurrentBegin())
      prevParent();
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    --(*this);
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
    return this->isEnd == __x.isEnd || this->current == __x.current;
  }

  bool operator!=(const _Self& __x) const {
    return !this->operator ==(__x);
  }

  _ValueIter getCurrentBegin() {
    return (*parentCurrent).begin();
  }

  _ValueIter getCurrentEnd() {
    return (*parentCurrent).end();
  }
};

template<
  typename Tparent,
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct MultiIter : public MultiIterBase<Tparent, Tcontainer, Tallocator> {
  typedef Tparent _Parent;
  typedef MultiIterBase<_Parent, Tcontainer,Tallocator> _Base;

  typedef typename _Parent::iterator _ValueIter;
  typedef typename Tcontainer<Tparent, Tallocator<Tparent> >::iterator _SuperIter;

  typedef typename _ValueIter::difference_type difference_type;
  typedef typename _ValueIter::iterator_category iterator_category;
  typedef typename _ValueIter::value_type value_type;
  typedef typename _ValueIter::pointer pointer;
  typedef typename _ValueIter::reference reference;

  explicit MultiIter(_SuperIter begin, _SuperIter end) :
    _Base(begin,end) {}

  MultiIter(const MultiIter& other) : _Base(other) {}
};

template<
  typename Tparent,
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct MultiConstIter : public MultiIterBase<Tparent, Tcontainer, Tallocator> {
  typedef Tparent _Parent;
  typedef MultiIterBase<_Parent, Tcontainer,Tallocator> _Base;

  typedef typename _Parent::iterator _ValueIter;
  typedef typename Tcontainer<Tparent, Tallocator<Tparent> >::iterator _SuperIter;

  typedef typename _ValueIter::difference_type difference_type;
  typedef typename _ValueIter::iterator_category iterator_category;
  typedef typename _ValueIter::value_type value_type;
  typedef typename _ValueIter::pointer pointer;
  typedef typename _ValueIter::reference reference;

  explicit MultiConstIter(_SuperIter begin, _SuperIter end) :
    _Base(begin,end) {}

  MultiConstIter(const MultiConstIter& other) : _Base(other) {}
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct SegmentWise {
  typedef typename Tcontainer<Point, Tallocator<Point> >::iterator _ParentIter;
  typedef SegmentWise<Tcontainer, Tallocator> _Self;

  _ParentIter begin;
  _ParentIter end;
  _ParentIter last;
  _ParentIter current;
  bool isEnd;

  explicit SegmentWise() : isEnd(true) {
  }

  explicit SegmentWise(_ParentIter parentBegin, _ParentIter parentEnd) :
    begin(parentBegin), end(parentEnd), last(parentBegin), isEnd(parentBegin == parentEnd){
    // check we are having either no or at least 2 points
    if(!isEnd)
      current = ++parentBegin;
    else
      current = parentEnd;
    if(current == parentEnd)
      isEnd = true;
  }

  _Self& operator++() {
    last = current++;
    if(current == end)
      isEnd= true;
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    //FIXME track end only once
    ++(*this);
    return __tmp;
  }

  _Self& operator--() {
    last = current--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
    return this->isEnd == __x.isEnd || this->current == __x.current;
  }

  bool operator!=(const _Self& __x) const {
    return !this->operator ==(__x);
  }
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct SegmentWiseIterator :
  public SegmentWise<Tcontainer,Tallocator> {
    typedef SegmentWiseIterator<Tcontainer,Tallocator> _Self;
    typedef SegmentWise<Tcontainer,Tallocator> _Base;

    typedef typename Tcontainer<Point, Tallocator<Point> >::iterator _ParentIter;
    typedef typename Tcontainer<Segment, Tallocator<Segment> >::iterator _ValueIter;

    typedef typename _ValueIter::difference_type difference_type;
    typedef typename _ValueIter::iterator_category iterator_category;
    typedef typename _ValueIter::value_type value_type;
    typedef typename _ValueIter::pointer pointer;
    typedef typename _ValueIter::reference reference;

  explicit SegmentWiseIterator() :
    _Base()
     {}
  explicit SegmentWiseIterator(_ParentIter parentBegin, _ParentIter  parentEnd) :
    _Base(parentBegin, parentEnd)
     {}

  reference operator*() const {
    //FIXME
    return * new Segment(*_Base::last, *_Base::current);
  }

  pointer const operator->() const {
    //FIXME
    return new Segment(*_Base::last, *_Base::current);
  }
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
struct SegmentWiseConstIterator :
  public SegmentWise<Tcontainer,Tallocator> {
    typedef SegmentWiseIterator<Tcontainer,Tallocator> _Self;
    typedef SegmentWise<Tcontainer,Tallocator> _Base;

    typedef typename Tcontainer<Point, Tallocator<Point> >::const_iterator _ParentIter;
    typedef typename Tcontainer<Segment, Tallocator<Segment> >::const_iterator _ValueIter;

    typedef typename _ValueIter::difference_type difference_type;
    typedef typename _ValueIter::iterator_category iterator_category;
    typedef typename _ValueIter::value_type value_type;
    typedef typename _ValueIter::pointer pointer;
    typedef typename _ValueIter::reference reference;

  explicit SegmentWiseConstIterator() :
      _Base()
  {}
  explicit SegmentWiseConstIterator(_ParentIter parentBegin, _ParentIter  parentEnd) :
      _Base(parentBegin, parentEnd)
  {}

  reference operator*() const {
    //FIXME
    return * new Segment(*_Base::last, *_Base::current);
  }

  pointer const operator->() const {
    //FIXME
    return new Segment(*_Base::last, *_Base::current);
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
