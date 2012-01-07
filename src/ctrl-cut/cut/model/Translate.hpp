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


#ifndef CLIP_HPP_
#define CLIP_HPP_

#include "util/Logger.hpp"
#include "cut/geom/Geometry.hpp"

#include <boost/foreach.hpp>

namespace trans = boost::geometry::strategy::transform;

template<
  typename TpointRangeOutputIterator
>
class TranslateFunc {
private:
  TpointRangeOutputIterator sink;
  trans::translate_transformer<Point, Point> transformer;

public:
  TranslateFunc(TpointRangeOutputIterator sink, const Point& translate) :
    sink(sink), transformer(translate.x, translate.y) {
  }

  TranslateFunc(const TranslateFunc& other) :
    sink(other.sink), transformer(other.transformer)
  {}

  template<typename TpointRange>
  void operator()(TpointRange& pointRange) {
    typename boost::remove_const<TpointRange>::type translated = pointRange.make();
    boost::geometry::transform(pointRange, translated, this->transformer);
    *sink++ = translated;
  }
};

template <class TpointRangeOutputIterator>
class TranslateSink : public boost::function_output_iterator<TranslateFunc<TpointRangeOutputIterator> > {
public:
  typedef boost::function_output_iterator<TranslateFunc<TpointRangeOutputIterator> > _Base;
  typedef std::output_iterator_tag iterator_category;
  typedef void value_type;
  typedef void difference_type;
  typedef void pointer;
  typedef void reference;

  explicit TranslateSink() :
    _Base() {};

  TranslateSink(TpointRangeOutputIterator& pointRangeSink, const Point& translate) :
    _Base(TranslateFunc<TpointRangeOutputIterator>(pointRangeSink, translate))
      {};

  TranslateSink(const TranslateSink& other) :
    _Base(other)
      {};
};


template<
  typename TmultiPointRange
>
void translate(TmultiPointRange& src, TmultiPointRange& sink, const Point& translate) {
  AddSink<TmultiPointRange> addSink(sink);
  TranslateSink<AddSink<TmultiPointRange> > transSink(addSink,translate);
  BOOST_FOREACH(const Path& path, src) {
    *transSink++ = path;
  }
}
#endif /* CLIP_HPP_ */
