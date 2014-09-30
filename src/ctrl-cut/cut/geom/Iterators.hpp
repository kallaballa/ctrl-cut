/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ITERATORS_HPP_
#define ITERATORS_HPP_

#include "cut/geom/Geometry.hpp"
#include <vector>
#include <stddef.h>

template<typename TparentIter,typename TvalueIter>
struct MultiIterBase {
  typedef MultiIterBase<TparentIter, TvalueIter> _Self;
  typedef TparentIter _SuperIter;
  typedef TvalueIter _ValueIter;

  _SuperIter parentBegin;
  _SuperIter parentEnd;
  _SuperIter parentCurrent;
  _ValueIter current;
  bool isEnd;

  explicit MultiIterBase() : isEnd(true) {}

  explicit MultiIterBase(_SuperIter begin, _SuperIter end) :
    parentBegin(begin), parentEnd(end), parentCurrent(begin), isEnd(begin == end) {
    if(!isEnd)
      current = getCurrentBegin();
  }

  MultiIterBase(const _Self& other) :
    parentBegin(other.parentBegin), parentEnd(other.parentEnd), parentCurrent(other.parentCurrent), current(other.current), isEnd(other.isEnd)
  {}

  void nextParent() {
    if(!isEnd) {
      if(parentCurrent++ == parentEnd) {
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

  bool operator==(const _Self& __x) const {
	//make sure not to compare singular iterators (if isEnd is true the iterators are empty)
	return (this->isEnd && __x.isEnd) || (!this->isEnd && !__x.isEnd && (this->current == __x.current));
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
  typename TparentIter
>
struct MultiIter :
  public MultiIterBase<
    TparentIter,
    typename TparentIter::value_type::const_iterator
  > {
  typedef MultiIter<TparentIter> _Self;
  typedef TparentIter _SuperIter;
  typedef typename _SuperIter::value_type::const_iterator _ValueIter;
  typedef MultiIterBase<_SuperIter, _ValueIter> _Base;

  typedef typename _ValueIter::difference_type difference_type;
  typedef typename _ValueIter::iterator_category iterator_category;
  typedef typename _ValueIter::value_type value_type;
  typedef typename _ValueIter::pointer pointer;
  typedef typename _ValueIter::reference reference;

  explicit MultiIter() :
    _Base()
     {}
  explicit MultiIter(_SuperIter begin, _SuperIter end) :
    _Base(begin,end) {}

  typename _ValueIter::reference operator*() const {
    return *_Base::current;
  }

  typename _ValueIter::pointer operator->() const {
    return &*_Base::current;
  }

  _Self&
  operator++() {
    _Base::current++;
    if(_Base::current == _Base::getCurrentEnd())
      _Base::nextParent();
    return *this;
  }

  _Self operator++(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  _Self&
  operator--() {
    _Base::current--;
    if(_Base::current == _Base::getCurrentBegin())
      _Base::prevParent();
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    --(*this);
    return __tmp;
  }
};

template<typename TparentIter>
struct SegmentWise {
  typedef TparentIter _PointIter;
  typedef SegmentWise<TparentIter> _Self;

  _PointIter begin;
  _PointIter end;
  _PointIter previous;
  _PointIter current;
  bool isEnd;

  explicit SegmentWise() : isEnd(true) {
  }

  explicit SegmentWise(_PointIter parentBegin, _PointIter parentEnd) :
    begin(parentBegin), end(parentEnd), previous(parentBegin), isEnd(parentBegin == parentEnd){
    // check we are having either no or at least 2 points
    if(!isEnd)
      current = ++parentBegin;
    else
      current = parentEnd;
    if(current == parentEnd)
      isEnd = true;
  }

  _Self& operator++() {
    previous = current++;
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
    previous = current--;
    return *this;
  }

  _Self operator--(int) {
    _Self __tmp = *this;
    ++(*this);
    return __tmp;
  }

  bool operator==(const _Self& __x) const {
	//make sure not to compare singular iterators (if isEnd is true the iterators are empty)
	return (this->isEnd && __x.isEnd) || (!this->isEnd && !__x.isEnd && (this->current == __x.current));
  }

  bool operator!=(const _Self& __x) const {
    return !this->operator ==(__x);
  }
};

template<typename TparentIter>
struct SegmentWiseIterator :
  public SegmentWise<TparentIter> {
    typedef TparentIter _ParentIter;
    typedef SegmentWiseIterator<_ParentIter> _Self;
    typedef SegmentWise<_ParentIter> _Base;

    typedef ptrdiff_t difference_type;
    typedef typename TparentIter::iterator_category iterator_category;
    typedef const Segment value_type;
    typedef const SegmentPtr pointer;
    typedef const SegmentPtr reference;

  explicit SegmentWiseIterator() :
    _Base()
  {}
  explicit SegmentWiseIterator(_ParentIter parentBegin, _ParentIter  parentEnd) :
    _Base(parentBegin, parentEnd)
  {}

  reference operator*() const {
    return SegmentPtr(new Segment(*_Base::previous, *_Base::current));
  }

  pointer operator->() const {
    return SegmentPtr(new Segment(*_Base::previous, *_Base::current));
  }
};


template<
  typename TmultiPointIter
>
struct MultSegmentWiseIterator
{
  typedef MultSegmentWiseIterator<TmultiPointIter> _Self;
  typedef TmultiPointIter _ParentIter;
  typedef SegmentWiseIterator<typename TmultiPointIter::value_type::const_iterator> _ValueIter;

  typedef typename _ValueIter::difference_type difference_type;
  typedef typename _ValueIter::iterator_category iterator_category;
  typedef typename _ValueIter::value_type value_type;
  typedef typename _ValueIter::pointer pointer;
  typedef typename _ValueIter::reference reference;


  _ParentIter parentBegin;
  _ParentIter parentEnd;
  _ParentIter parentCurrent;
  _ValueIter current;
  bool isEnd;

  explicit MultSegmentWiseIterator() : isEnd(true) {}

  explicit MultSegmentWiseIterator(_ParentIter begin, _ParentIter end) :
    parentBegin(begin), parentEnd(end), parentCurrent(begin), isEnd(begin == end) {
    if(!isEnd)
      current = getCurrentBegin();
  }

  MultSegmentWiseIterator(const _Self& other) :
    parentBegin(other.parentBegin), parentEnd(other.parentEnd), parentCurrent(other.parentCurrent), current(other.current), isEnd(other.isEnd)
  {}

  typename _ValueIter::reference operator*() const {
    return *current;
  }

  typename _ValueIter::pointer operator->() const {
    return &*current;
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

  void nextParent() {
    if(!isEnd) {
      ++parentCurrent;
      if(parentCurrent == parentEnd) {
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

  bool operator==(const _Self& __x) const {
	//make sure not to compare singular iterators (if isEnd is true the iterators are empty)
    return (this->isEnd && __x.isEnd) || (!this->isEnd && !__x.isEnd && (this->current == __x.current));
  }

  bool operator!=(const _Self& __x) const {
    return !this->operator ==(__x);
  }

  _ValueIter getCurrentBegin() {
    return _ValueIter((*parentCurrent).begin(), (*parentCurrent).end());
  }

  _ValueIter getCurrentEnd() {
    return _ValueIter((*parentCurrent).end(), (*parentCurrent).end());
  }
};
#endif /* ITERATORS_HPP_ */
