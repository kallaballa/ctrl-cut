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

#ifndef CREATESINK_HPP_
#define CREATESINK_HPP_

#include <boost/function_output_iterator.hpp>
#include <boost/foreach.hpp>

template<typename Tgraph, typename TpointRange>
struct CreateFunc {
  Tgraph* graph;

  CreateFunc(Tgraph& graph) : graph(&graph) {
//FIXME concept check
  }

  void operator()(const TpointRange& pointRange) {
    graph->create(pointRange);
  }
};

template <class Tgraph, class TpointRange>
class CreateSink : public boost::function_output_iterator<CreateFunc<Tgraph, TpointRange> > {
public:
  typedef boost::function_output_iterator<CreateFunc<Tgraph,TpointRange> > _Base;
  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  explicit CreateSink() :
    _Base() {};

  CreateSink(Tgraph& graph) :
    _Base(CreateFunc<Tgraph,TpointRange>(graph))
      {};

  CreateSink(const CreateSink& other) :
    _Base(other)
      {};
};


template<
  typename TmultiPointRange,
  typename Tgraph
>

void createGeometryGraph(const TmultiPointRange& multiPointRange, Tgraph& graph) {
  CreateSink<Tgraph,typename TmultiPointRange::value_type> sink(graph);
  BOOST_FOREACH(typename TmultiPointRange::value_type geom, multiPointRange) {
    *sink++ = geom;
  }
}

#endif /* CREATESINK_HPP_ */
