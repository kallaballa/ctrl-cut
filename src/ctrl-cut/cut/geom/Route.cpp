#include "Route.h"
#include "cut/graph/Traverse.h"
#include "util/Measurement.h"


Route::Route(const CutModel& model) : CutModel (model){}

bool Route::append(const Segment& seg) {
  std::cerr << *this << std::endl;
  if (this->empty()) {
    return this->create(seg);
  } else if(!this->frontStrings().append(seg) && !this->backStrings().append(seg)) {
    for(StringIter it = beginStrings(); it != endStrings(); it++) {
      if((*it).append(seg))
        return true;
    }
    return false;
  }
  return true;
}

CutModel::iterator Route::begin() {
  return Route::iterator(this->beginStrings());
}

CutModel::const_iterator Route::begin() const  {
  return Route::const_iterator(this->beginStrings());
}

void Route::push_front(const Segment& seg) {
  //FIXME CutModel::create(seg);
  SegmentString* string = new SegmentString(*this);
  string->append(seg);
  strings.push_front(string);
}

void Route::push_back(const Segment& seg) {
  //FIXME CutModel::create(seg);
  SegmentString* string = new SegmentString(*this);
  string->create(seg);
  strings.push_back(string);
}

void Route::remove(Segment& seg) {
  assert(!"not implemented");
}

CutModel::iterator Route::erase(iterator it) {
  assert(!"not implemented");
  return it;
}

bool Route::empty() const {
  if(emptyStrings())
    return true;

  for(StringConstIter it = beginStrings(); it != endStrings(); it++) {
    if(!(*it).empty())
      return false;
  }

  return true;
}

void Route::clear() {
  for(StringIter it = beginStrings(); it != endStrings(); it++) {
    delete &*it;
  }
}

void Route::copy(const Route& other) {
  CutModel::copy(other);
  for(StringConstIter it = other.beginStrings(); it != other.endStrings(); it++) {
    this->strings.push_back(new SegmentString(*it));
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

    dump_linestrings("innerouter.xml", route.beginStrings(), route.endStrings());
  } else
    assert(false);

  return route;
}

