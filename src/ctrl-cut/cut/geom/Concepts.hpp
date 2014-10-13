/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef CONCEPTS_H_
#define CONCEPTS_H_

#include <boost/concept_check.hpp>
#include "Geometry.hpp"

template <class X>
struct PointInputIterator
  : boost::InputIteratorConcept<X>
{
 private:
    typedef std::iterator_traits<X> t;
 public:
    typedef typename t::value_type value_type;
    BOOST_CONCEPT_ASSERT((boost::Convertible<value_type, Point>));
};

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

template <class x>
struct PointOutputIterator
  : boost::OutputIteratorConcept<x,Segment>
{

};
#endif /* CONCEPTS_H_ */
