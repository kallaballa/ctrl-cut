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

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include <boost/concept_check.hpp>
#include <cut/geom/Concepts.hpp>

template<typename TinputIterator, typename ToutputIterator>
class Processor {
  TinputIterator begin;
  TinputIterator end;
  ToutputIterator sink;

  Processor(TinputIterator begin, TinputIterator end, ToutputIterator sink) :
    begin(begin), end(end), sink(sink) {
    BOOST_CONCEPT_ASSERT((boost::InputIterator<TinputIterator>));
    BOOST_CONCEPT_ASSERT((SegmentOutputIterator<ToutputIterator>));
  }

  virtual void operator()() = 0;
};

template<typename TsegmentInputIterator, typename TsegmentOutputIterator>
class SegmentProcessor : public Processor<TsegmentInputIterator, TsegmentOutputIterator> {
  TsegmentInputIterator begin;
  TsegmentInputIterator end;
  TsegmentOutputIterator sink;

  SegmentProcessor(TsegmentInputIterator begin, TsegmentInputIterator end,
      TsegmentOutputIterator sink) :
    begin(begin), end(end), sink(sink) {
    BOOST_CONCEPT_ASSERT((SegmentInputIterator<TsegmentInputIterator>));
    BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));
  }

  virtual void operator()() = 0;
};

template<typename TpointInputIterator, typename TpointOutputIterator>
class PointProcessor : public Processor<TpointInputIterator, TpointOutputIterator>{
  TpointOutputIterator begin;
  TpointOutputIterator end;
  TpointOutputIterator sink;

  PointProcessor(TpointInputIterator begin, TpointInputIterator end, TpointOutputIterator sink) :
    begin(begin), end(end), sink(sink) {
    BOOST_CONCEPT_ASSERT((PointInputIterator<TpointInputIterator>));
    BOOST_CONCEPT_ASSERT((PointOutputIterator<TpointOutputIterator>));
  }

  virtual void operator()() = 0;
};

#endif /* PROCESSOR_H_ */
