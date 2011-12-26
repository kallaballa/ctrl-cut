#include "Route.h"
#include "cut/graph/Traverse.h"
#include "util/Measurement.h"

Route::Route(const CutModel& model) : CutModel (model){}

bool Route::append(const Segment& seg) {
  if (this->emptyStrings()) {
    return this->create(seg);
  } else if(!this->frontStrings().create(seg) && !this->backStrings().create(seg)) {
    for(StringIter it = beginStrings(); it != endStrings(); it++) {
      if((*it).create(seg))
        return true;
    }
    return false;
  }
  return true;
}

bool Route::create(const Segment& seg) {
  CutModel::create(seg);
  SegmentString string(*this);
  string.create(seg);
  strings.push_back(string);
  return true;
}

void Route::copy(const Route& other) {
  CutModel::copy(other);
  for(StringConstIter it = other.beginStrings(); it != other.endStrings(); it++) {
    this->strings.push_back(*it);
  }
}

Route& Route::make(CutModel& model) {
  Route& route = * new Route(model);
  const Point&  pos = model.get(CutSettings::CPOS);
  SegmentList::iterator segmentsFirst;
  SegmentList::iterator segmentsLast;
  SegmentList translated;

  if(pos.x != 0 || pos.y != 0) {
    translate(translated, model.begin(), model.end(), pos);
    segmentsFirst = translated.begin();
    segmentsLast = translated.end();
  } else{
    segmentsFirst = model.begin();
    segmentsLast = model.end();
  }

  CutSettings::Optimize optimize = model.get(CutSettings::OPTIMIZE);
  string datadir = model.get(DocumentSettings::DATA_DIR);
  string basename = model.get(DocumentSettings::BASENAME);

  if(optimize == CutSettings::SIMPLE) {
    //FIXME dump_linestrings("simple", segmentsFirst, segmentsLast);
  } else if(optimize == CutSettings::SHORTEST_PATH) {
    Route join(model);
    make_linestrings(join,segmentsFirst, segmentsLast);
    travel_linestrings(route, join.beginStrings(), join.beginStrings());

    //FIXME dump_linestrings("join", segmentsFirst, segmentsLast);
  } else if(optimize == CutSettings::INNER_OUTER) {
    traverse_onion(route, segmentsFirst, segmentsLast);

    //FIXME dump_linestrings("innerouter", segmentsFirst, segmentsLast);
  } else
    assert(false);

  return route;
}

