#ifndef FILTERCHAIN_H_
#define FILTERCHAIN_H_

#include <list>
#include <string>
#include "Filter.h"

using std::list;
using std::string;

class FilterChain {
 public:
  FilterChain();
  virtual ~FilterChain();
  bool evaluate();
  void filterPass(LaserPass *pass);

 private:
  typedef list<Filter*> FilterList;
  FilterList chain;
};

#endif /* FILTERCHAIN_H_ */
