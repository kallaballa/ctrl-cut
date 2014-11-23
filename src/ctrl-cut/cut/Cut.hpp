/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef CUT_H_
#define CUT_H_

#include <cut/operations/NearestPathSorting.hpp>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "config/DocumentSettings.hpp"
#include "config/CutSettings.hpp"

#include "svg/SvgPlot.hpp"

#include "cut/geom/Route.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"

#include "cut/operations/Clip.hpp"
#include "cut/operations/Explode.hpp"
#include "cut/operations/Planar.hpp"
#include "cut/operations/Reduce.hpp"
#include "cut/operations/Translate.hpp"
#include "cut/operations/Deonion.hpp"
#include "cut/operations/Traveller.hpp"

#include <limits>
#include <memory>

template<
template<typename,typename> class Tcontainer = std::vector,
template<typename> class Tallocator = std::allocator
>
class CutImpl : public RouteImpl<Tcontainer, Tallocator> {
public:
  typedef CutImpl<Tcontainer, Tallocator> _Self;
  typedef RouteImpl<Tcontainer, Tallocator> Route_t;
  CutImpl(DocumentSettings& parentSettings) :
    Route_t(parentSettings)
  { }

  CutImpl(const CutSettings& settings) :
    Route_t(settings)
  { }

  ~CutImpl() {
    this->clear();
  }

  void normalize() {
    typedef CutImpl<Tcontainer, Tallocator> CutPtr;
    typedef DocumentSettings DS;
    Distance width = this->get(DS::WIDTH);
    Distance height = this->get(DS::HEIGHT);
    int dpi = this->get(DS::RESOLUTION);
    string filename = this->get(DS::FILENAME);

    Distance reduceMax = this->get(CutSettings::REDUCE);

    CutPtr& cut = *this;
    plot_svg(cut, filename + "_input");

    CutPtr exploded = make_from(cut);
    CutPtr clipped = make_from(cut);
    CutPtr reduced = make_from(cut);

    explode(cut, exploded);
    plot_svg(exploded, filename + "_exploded");

    clip(exploded, clipped, Box(Point(0,0), Point(width.in(PX),height.in(PX))));
    plot_svg(clipped, filename + "_clipped");

    reduce(clipped, reduced, reduceMax.in(PX));
    plot_svg(reduced, filename + "_reduced");
    this->clear();
    (*this) = reduced;
  }

  void sort() {
    typedef CutImpl<Tcontainer, Tallocator> CutPtr;
    string filename = this->get(DocumentSettings::FILENAME);
    CutPtr planar_faces = make_from(*this);
    CutPtr sorted = make_from(*this);

    make_planar_faces(*this, planar_faces);
    plot_svg(planar_faces, filename + "_planar_faces");


   if(this->get(CutSettings::SORT) == CutSettings::INNER_OUTER)
      traverse_onion(planar_faces, sorted);
    else if(this->get(CutSettings::SORT) == CutSettings::SHORTEST_PATH)
      nearest_path_sorting(planar_faces, sorted);
    else if(this->get(CutSettings::SORT) == CutSettings::TSP)
      travel(planar_faces, sorted);

    plot_svg(sorted, filename + "_sorted");
    this->clear();
    (*this) = sorted;
  }

  void translate() {
    CutImpl<Tcontainer, Tallocator> translated = make_from(*this);
    const Point&  pos = this->get(CutSettings::CPOS);
    string filename = this->get(DocumentSettings::FILENAME);

    translateTo(*this, translated, pos);
    plot_svg(translated, filename + "_translate");
    this->clear();
    (*this) = translated;
    this->put(CutSettings::CPOS, Point());
  }

  void crop() {
    CutImpl<Tcontainer, Tallocator> cropped = make_from(*this);
    const Point&  pos = this->get(CutSettings::CPOS);
    if(pos != Point(0,0))
      this->translate();

    Coord_t minx = std::numeric_limits<Coord_t>::max();
    Coord_t miny = std::numeric_limits<Coord_t>::max();
    for(typename Route_t::iterator it = this->begin(); it != this->end(); ++it) {
      const Path& pt = *it;

      for(Path::const_iterator itp = pt.begin(); itp != pt.end(); ++itp) {
        const Point& p = *itp;
        minx = std::min(p.x, minx);
        miny = std::min(p.y, miny);
      }
    }

    translateTo(*this,cropped, Point(-minx, -miny));

    cropped.put(CutSettings::CPOS, Point(minx, miny));
    this->clear();
    (*this) = cropped;
  }

  void operator=(const _Self& other) {
    this->settings = other.settings;
    this->clear();
    Route_t::operator=(other);
  }
};

typedef CutImpl<std::vector, std::allocator> Cut;
typedef std::shared_ptr<Cut> CutPtr;


inline MultiSegmentView<const Cut> segments(const CutPtr& cut) {
  return MultiSegmentView<const Cut>(*cut.get());
}

std::vector<CutPtr> load_cuts(DocumentSettings& ds, std::istream &input);
std::vector<CutPtr> load_cuts(DocumentSettings& ds, const std::string &filename);
Box find_bounding_box(Cut& cut);

#endif /* CUT_H_ */
