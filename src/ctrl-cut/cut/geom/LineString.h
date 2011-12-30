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

#ifndef LINESTRING_H_
#define LINESTRING_H_

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>
#include "Segment.h"
#include "Views.h"
#include "config/CutSettings.h"

template<
 template<typename,typename> class Tcontainer = std::list,
 template<typename> class Tallocator = std::allocator
>
struct SegmentViewImpl : public View<
   SegmentWiseIterator<Tcontainer, Tallocator>,
   SegmentWiseConstIterator<Tcontainer, Tallocator>,
   Tcontainer<Point, Tallocator<Point> >
   > {

 SegmentViewImpl(Tcontainer<Point, Tallocator<Point> >& container) : View<
 SegmentWiseIterator<Tcontainer, Tallocator>,
 SegmentWiseConstIterator<Tcontainer, Tallocator>,
 Tcontainer<Point, Tallocator<Point> >
 >(container) {
 }
};

template<
  template<typename,typename> class Tcontainer = std::list,
  template<typename> class Tallocator = std::allocator
>
class LineStringImpl : public Tcontainer<Point, Tallocator<Point> > {
public:
  typedef Tcontainer<Point, Tallocator<Point> > _BaseClass;
  typedef typename Tcontainer<Point, Tallocator<Point> >::iterator iterator;
  typedef typename Tcontainer<Point, Tallocator<Point> >::const_iterator const_iterator;

  //only copy settings and inherit the parent settings
  LineStringImpl(CutSettings& parentSettings) : settings(parentSettings) {}

  LineStringImpl(const LineStringImpl& other) : _BaseClass(other), settings(other.settings) {
  }

  bool append(const int32_t& inX,const int32_t& inY,const int32_t& outX,const int32_t& outY) {
    return this->append(Point(inX, inY), Point(outX, outY));
  }

  bool append(const Point&  p1, const Point&  p2) {
    return this->append(Segment(p1,p2));
  }

  virtual bool append(const Segment& seg) {
    //ignore zero length segments
    if (seg.first == seg.second)
      return false;
    else
      return this->appendBack(seg) || this->appendFront(seg);
  }

  bool appendBack(const Segment& seg) {
    if (this->empty()) {
      this->push_back(seg.first);
      this->push_back(seg.second);
      return true;
    } else {
      const Point& last = this->back();
      if (last == seg.first)
        this->push_back(seg.second);
      else if (last == seg.second)
        this->push_back(seg.first);
    }
    return false;
  }

  bool appendFront(const Segment& seg) {
    if (this->empty()) {
      this->push_front(seg.first);
      this->push_front(seg.second);
      return true;
    } else {
      const Point& first = this->front();
      if (first == seg.first)
        this->push_front(seg.second);
      else if (first == seg.second)
        this->push_front(seg.first);
    }
    return false;
  }

  void operator=(const LineStringImpl& other) {
    this->copy(other);
  }

  //deep copy
  void copy(const LineStringImpl& other) {
    (*static_cast<_BaseClass*>(this)) = other;
    this->settings = other.settings;
  }

  friend std::ostream& operator<<(std::ostream &os, LineStringImpl& string)  {
    os << "<string>" << std::endl;
    os << "  <points>" << std::endl;
    for(LineStringImpl::iterator it=string.begin(); it != string.end(); ++it)
      os << (*it) << std::endl;
    os << std::endl;
    os << "  </points>" << std::endl;
    os << "  <segments>" << std::endl;
    SegmentViewImpl<std::list> sv(string);
    for(SegmentViewImpl<std::list>::iterator it=sv.begin(); it != sv.end(); ++it)
      os << (*it) << std::endl;
    os << std::endl;
    os << "  </segments>" << std::endl;
    os << "</route>" << std::endl;
    return os;
  }
private:
  SegmentSettings settings;
};

typedef LineStringImpl<std::list> LineString;
typedef SegmentViewImpl<std::list> SegmentView;
BOOST_GEOMETRY_REGISTER_LINESTRING(LineString)
#endif /* LINESTRING_H_ */
