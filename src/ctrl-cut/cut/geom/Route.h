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

#ifndef ROUTE_H_
#define ROUTE_H_

#include <algorithm>
#include "Geometry.h"
#include "Views.h"
#include "cut/geom/LineString.h"
#include "config/CutSettings.h"

template<
  template<typename , typename > class Tcontainer = std::list,
  template<typename > class Tallocator = std::allocator
>
struct MultiLineString : Tcontainer<LineStringImpl<Tcontainer, Tallocator>, Tallocator<LineStringImpl<Tcontainer, Tallocator> > >
{};

template<
  template<typename , typename > class Tcontainer = std::list,
  template<typename > class Tallocator = std::allocator
>
struct MultiSegmentViewContainer :
  Tcontainer<
    SegmentViewImpl<Tcontainer, Tallocator>,
    Tallocator<
    SegmentViewImpl<Tcontainer, Tallocator>
    >
  >
{};

template<
  template<typename , typename > class Tcontainer = std::list,
  template<typename > class Tallocator = std::allocator
>
struct MultiSegmentViewImpl :
  public View<
    MultiIter<
      SegmentViewImpl<Tcontainer, Tallocator>,
      Tcontainer,
      Tallocator
    >,
    MultiConstIter<
      SegmentViewImpl<Tcontainer, Tallocator>,
      Tcontainer,
      Tallocator
    >,
    MultiSegmentViewContainer<Tcontainer,Tallocator>
  > {
    MultiSegmentViewContainer<Tcontainer,Tallocator> views;

    template<typename Trange>
    MultiSegmentViewImpl(Trange& strings) : View<
          MultiIter<
            SegmentViewImpl<Tcontainer, Tallocator>,
            Tcontainer,
            Tallocator
          >,
          MultiConstIter<
            SegmentViewImpl<Tcontainer, Tallocator>,
            Tcontainer,
            Tallocator
          >,
          MultiSegmentViewContainer<Tcontainer,Tallocator>
        >(views)
    {
      for (typename Trange::iterator it = strings.begin(); it != strings.end(); ++it) {
        views.push_back(SegmentViewImpl<Tcontainer,Tallocator>(*it));
      }
    };
};

template<
  template<typename , typename > class Tcontainer = std::list,
  template<typename > class Tallocator = std::allocator
>
struct MultiPointViewImpl :
  public View<
    MultiIter<
      LineStringImpl<Tcontainer, Tallocator>,
      Tcontainer,
      Tallocator
    >,
    MultiConstIter<
      LineStringImpl<Tcontainer, Tallocator>,
      Tcontainer,
      Tallocator
    >,
    MultiLineString<Tcontainer, Tallocator>
  > {

    MultiPointViewImpl(MultiLineString<Tcontainer, Tallocator>& strings) : View<
        MultiIter<
          LineStringImpl<Tcontainer, Tallocator>,
          Tcontainer,
          Tallocator
        >,
        MultiConstIter<
          LineStringImpl<Tcontainer, Tallocator>,
          Tcontainer,
          Tallocator
        >,
        MultiLineString<Tcontainer, Tallocator>
      > (strings)
    {};
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
class RouteImpl {
private:
  typedef RouteImpl<Tcontainer, Tallocator> _Self;
  typedef LineStringImpl<Tcontainer, Tallocator> _LineString;
  typedef MultiLineString<Tcontainer, Tallocator> _MultiLineStrings;
  _MultiLineStrings strings;
  CutSettings settings;
public:
  typedef typename _MultiLineStrings::iterator iterator;
  typedef typename _MultiLineStrings::const_iterator const_iterator;

  RouteImpl() {}
  RouteImpl(DocumentSettings& parentSettings) : settings(parentSettings) {}

  iterator begin() {
    return this->strings.begin();
  }
  const_iterator begin() const {
    return this->strings.begin();
  }
  iterator end()  {
    return this->strings.end();
  }
  const_iterator end() const {
    return this->strings.end();
  }
  typename iterator::reference front() {
    return this->strings.front();
  }
  typename iterator::reference back() {
    return this->strings.back();
  }
  typename iterator::reference front() const {
    return this->strings.front();
  }
  typename iterator::reference back() const {
    return this->strings.back();
  }

  void push_front(const Segment& seg) {
    _LineString string(this->settings);
    string.append(seg);
    strings.push_front(string);
  }

  void push_back(const Segment& seg) {
    _LineString string(this->settings);
    string.append(seg);
    strings.push_back(string);
  }

  bool append(const int32_t& inX,const int32_t& inY,const int32_t& outX,const int32_t& outY) {
    return this->append(Point(inX, inY), Point(outX, outY));
  }

  bool append(const Point&  p1, const Point&  p2) {
    return this->append(Segment(p1,p2));
  }

  virtual bool append(const Segment& seg) {
    if (this->empty()) {
      this->push_back(seg);
      return true;
    } else if(! (this->appendFront(seg) || this->appendBack(seg))) {
      for(iterator it = strings.begin(); it != strings.end(); ++it) {
        if((*it).append(seg))
          return true;
      }
      return false;
    }
    return true;
  }

  bool appendFront(const Segment& seg) {
    if (this->empty()) {
      this->push_front(seg);
      return true;
    } else  {
      return strings.front().appendFront(seg);
    }
  }

  bool appendBack(const Segment& seg) {
    if (this->empty()) {
      this->push_front(seg);
      return true;
    } else  {
      return strings.back().appendBack(seg);
    }
  }

  void clear() {
    strings.clear();
  }

  void copy(const RouteImpl& other) {
    this->settings = other.getSettings();
    for(iterator it = strings.begin(); it != strings.end(); ++it) {
      this->strings.push_back(*it);
    }
  }

  bool empty() const {
    return strings.empty();
  }

  size_t size() const {//FIXME
    return strings.size();
  }

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    settings.put(key,value);
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) const {
    return settings.get(key);
  }

  friend std::ostream& operator<<(std::ostream &os, RouteImpl& route)  {
    os << "<route>" << std::endl;
    for(RouteImpl::iterator it=route.begin(); it != route.end(); ++it)
      os << (*it) << std::endl;
    os << std::endl;
    os << "</route>" << std::endl;
    return os;
  }
};

#endif

typedef RouteImpl<std::list, std::allocator> Route;
typedef MultiSegmentViewImpl<std::list, std::allocator> MultiSegmentView;
typedef MultiPointViewImpl<std::list, std::allocator> MultiPointView;
