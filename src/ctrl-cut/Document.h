#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include<libgen.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>
#include "stdint.h"
#include <iostream>
#include "boost/format.hpp"

#include "util/PJL.h"
#include "engrave/Engrave.h"
#include "config/DocumentSettings.h"

#include "util/Util.h"
#include <cups/cups.h>
#include <cups/file.h>

#include "FileParser.h"
#include <boost/thread.hpp>
#include "svg/Svg2Ps.h"

using std::string;

class CutModel;
class Document{
public:
  enum Format {
    UNSPECIFIED,
    POSTSCRIPT,
    SVG,
    VECTOR,
    PBM
  };

  enum LoadType {
    ENGRAVING,
    CUT,
    BOTH
  };

  typedef std::list<CutModel*> CutList;
  typedef std::list<Engraving*> EngraveList;
  typedef CutList::iterator CutIt;
  typedef EngraveList::iterator EngraveIt;
  typedef CutList::const_iterator CutConstIt;
  typedef EngraveList::const_iterator EngraveConstIt;

  Document() {}
  virtual ~Document() {};

  CutList cutList;
  EngraveList engraveList;

  CutIt begin_cut() { return this->cutList.begin(); }
  CutConstIt begin_cut() const  { return this->cutList.begin(); }
  CutIt end_cut() { return this->cutList.end(); }
  CutConstIt end_cut() const  { return this->cutList.end(); }
  CutList::reference front_cut() { return this->cutList.front(); }
  CutList::reference back_cut() { return this->cutList.back(); }
  size_t size_cut() const { return this->cutList.size(); }
  bool empty_cut() const { return this->cutList.empty(); }

  EngraveIt begin_engrave() { return this->engraveList.begin(); }
  EngraveConstIt begin_engrave() const  { return this->engraveList.begin(); }
  EngraveIt end_engrave() { return this->engraveList.end(); }
  EngraveConstIt end_engrave() const  { return this->engraveList.end(); }
  EngraveList::reference front_engrave() { return this->engraveList.front(); }
  EngraveList::reference back_engrave() { return this->engraveList.back(); }
  size_t size_engrave() const { return this->engraveList.size(); }
  bool empty_engrave() const { return this->engraveList.empty(); }

  void optimize();
  void addCut(CutModel* cut);
  void addRaster(Engraving* raster);
  CutList getCuts() { return cutList; }

  void write(std::ostream &out);
  Document& preprocess();

  Format findFormat(const string& filename);
  bool load(const string& filename, LoadType load = BOTH, Format docFormat = UNSPECIFIED);
  DocumentSettings& getSettings() {
    return settings;
  };

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    settings.put(key,value);
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) const {
    return settings.get(key);
  }
private:
  DocumentSettings settings;
};

#endif /* DOCUMENT_H_ */
