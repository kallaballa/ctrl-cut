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

#ifndef PATH_H_
#define PATH_H_

#include "Views.hpp"
#include "config/CutSettings.hpp"

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/register/linestring.hpp>

template<
  template<typename,typename> class Tcontainer = std::vector,
  template<typename> class Tallocator = std::allocator
> //FIXME concept
class PathImpl : public GeometryGroup<Point, Tcontainer, Tallocator> {
public:
  typedef GeometryGroup<Point, Tcontainer, Tallocator> _Base;
  typedef typename _Base::iterator iterator;
  typedef typename _Base::const_iterator const_iterator;

  PathImpl() :
    _Base() {}
  PathImpl(const PathImpl& other) :
    _Base(other) {}

  void operator=(const PathImpl& other) {
    (*static_cast<_Base*>(this)) = other;
  }

  friend std::ostream& operator<<(std::ostream &os, PathImpl& path) {
    os << "<path>" << std::endl;
    os << "  <points>" << std::endl;
    for(typename PathImpl::const_iterator it=path.begin(); it != path.end(); ++it)
      os << (*it) << std::endl;
    os << std::endl;
    os << "  </points>" << std::endl;
    os << "  <segments>" << std::endl;
    SegmentView<PathImpl> sv(path);
    for(typename SegmentView<PathImpl>::const_iterator it=sv.begin(); it != sv.end(); ++it)
      os << (*it) << std::endl;
    os << std::endl;
    os << "  </segments>" << std::endl;
    os << "<path>" << std::endl;
    return os;
  }
};

typedef PathImpl<std::vector, std::allocator> Path;

inline static SegmentView<const Path> segments(const Path& path) {
  return SegmentView<const Path>(path);
}

BOOST_GEOMETRY_REGISTER_LINESTRING(Path)
#endif /* PATH_H_ */
