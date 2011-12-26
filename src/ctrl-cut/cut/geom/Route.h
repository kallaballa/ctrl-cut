#ifndef ROUTE_H_
#define ROUTE_H_

#include <algorithm>
#include "Geometry.h"
#include "cut/geom/SegmentString.h"

class CutModel;

class Route : public CutModel {
public:
  typedef std::list<SegmentString> StringList;
  typedef StringList::iterator StringIter;
  typedef StringList::const_iterator StringConstIter;

  Route(const CutModel& model);
  StringIter beginStrings() { return this->strings.begin(); }
  StringConstIter beginStrings() const  { return this->strings.begin(); }
  StringIter endStrings() { return this->strings.end(); }
  StringConstIter endStrings() const  { return this->strings.end(); }
  StringList::reference frontStrings()  { return this->strings.front(); }
  StringList::reference backStrings()  { return this->strings.back(); }
  StringList::const_reference frontStrings() const { return this->strings.front(); }
  StringList::const_reference backStrings() const { return this->strings.back(); }
  size_t numStrings() const { return this->strings.size(); }
  bool emptyStrings() const { return this->strings.empty(); }
  bool create(const Segment& seg);
  bool append(const Segment& seg);
  void copy(const Route& other);
  friend std::ostream& operator<<(std::ostream &os, Route& route)  {
    os << "<route>" << std::endl;
    for(Route::StringConstIter it=route.beginStrings(); it != route.endStrings(); ++it)
      os << (*it) << std::endl;
    os << "</route>" << std::endl;
    return os;
  }
  static Route& make(CutModel& model);
private:

  StringList strings;
};

#endif
