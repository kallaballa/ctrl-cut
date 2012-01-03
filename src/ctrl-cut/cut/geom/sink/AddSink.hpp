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
#ifndef ADDSINK_HPP_
#define ADDSINK_HPP_

#include <cut/geom/Algorithms.hpp>
#include <boost/function_output_iterator.hpp>

template <class TgeomContainer>
struct AddFunc {
  TgeomContainer* container;

  AddFunc() :
    container(NULL)
  {}
  AddFunc(TgeomContainer& container) :
    container(&container)
  {}

  template <class Tgeom>
  void operator()(const Tgeom& geom) const {
    add(*container,geom);
  }
};

template <class TgeomContainer>
class AddSink : public boost::function_output_iterator<AddFunc<TgeomContainer> > {
public:
  typedef boost::function_output_iterator<AddFunc<TgeomContainer> > _Base;
  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  explicit AddSink() :
    _Base() {};

  AddSink(TgeomContainer& container) :
    _Base(AddFunc<TgeomContainer>(container))
      {};

  AddSink(const AddSink& other) :
    _Base(other)
      {};
};

#endif /* ADDSINK_HPP_ */
