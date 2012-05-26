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

#include "Path.hpp"
#include "Views.hpp"
#include <config/CutSettings.hpp>

#include <algorithm>

template<
  typename Tgeom,
  template<typename , typename > class Tcontainer = std::vector,
  template<typename > class Tallocator = std::allocator
>
struct MultiGeometry : Tcontainer<Tgeom, Tallocator<Tgeom> >
{};

template<
  template<typename , typename > class Tcontainer = std::vector,
  template<typename > class Tallocator = std::allocator
>
struct MultiSegment : MultiGeometry<Segment, Tcontainer, Tallocator>
{};

template<
  template<typename , typename > class Tcontainer = std::vector,
  template<typename > class Tallocator = std::allocator
>
struct MultiLineString : MultiGeometry<Path, Tcontainer, Tallocator>
{};

template<
  template<typename,typename> class Tcontainer = std::vector,
  template<typename> class Tallocator = std::allocator
>
class RouteImpl {
private:
  typedef RouteImpl<Tcontainer, Tallocator> _Self;
  typedef PathImpl<Tcontainer, Tallocator> _LineString;
  typedef MultiLineString<Tcontainer, Tallocator> _MultiLineStrings;
  _MultiLineStrings paths;
public:
  CutSettings settings;
  typedef typename _MultiLineStrings::iterator iterator;
  typedef typename _MultiLineStrings::const_iterator const_iterator;
  typedef typename _MultiLineStrings::value_type value_type;

  RouteImpl() {}
  RouteImpl(DocumentSettings& parentSettings) : settings(parentSettings) {}
  RouteImpl(const CutSettings& settings) : settings(settings) {}

  iterator begin() {
    return this->paths.begin();
  }
  const_iterator begin() const {
    return this->paths.begin();
  }
  iterator end()  {
    return this->paths.end();
  }
  const_iterator end() const {
    return this->paths.end();
  }
  typename iterator::reference front() {
    return this->paths.front();
  }
  typename iterator::reference back() {
    return this->paths.back();
  }
  typename const_iterator::reference front() const {
    return this->paths.front();
  }
  typename const_iterator::reference back() const {
    return this->paths.back();
  }

  void clear() {
    paths.clear();
  }

  void insert(iterator pos, const Path& path) {
    paths.insert(pos, path);
  }

  void erase(iterator pos) {
    paths.erase(pos);
  }

  void remove(const Path& path) {
    paths.remove(path);
  }

  void push_back(const Path& path) {
    paths.push_back(path);
  }

  void copy(const RouteImpl& other) {
    this->settings = other.settings;
    for(iterator it = paths.begin(); it != paths.end(); ++it) {
      this->paths.push_back(*it);
    }
  }

  bool empty() const {
    return paths.empty();
  }

  size_t size() const {
    return paths.size();
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
    for(typename RouteImpl::iterator it=route.begin(); it != route.end(); ++it)
      os << (*it) << std::endl;
    os << std::endl;
    os << "</route>" << std::endl;
    return os;
  }
};

typedef RouteImpl<std::vector, std::allocator> Route;

inline MultiSegmentView<const Route> segments(const Route& route) {
  return MultiSegmentView<const Route>(route);
}

inline MultiPointView<const Route> points(const Route& route) {
  return MultiPointView<const Route>(route);
}
#endif


