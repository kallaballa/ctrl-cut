#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include "util/PJL.hpp"
#include "engrave/Engrave.hpp"
#include "cut/Cut.hpp"
#include "config/DocumentSettings.hpp"

#include "FileParser.hpp"
#include "util/Util.hpp"

#include<libgen.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>
#include "stdint.h"
#include <iostream>

#include <cups/cups.h>
#include <cups/file.h>
#include <boost/thread.hpp>
#include "boost/format.hpp"

using std::string;

class Document{
public:
  enum Format {
    UNSPECIFIED,
    POSTSCRIPT,
    SVG,
    VECTOR,
    PBM
  };

  typedef std::list<Cut*> CutList;
  typedef std::list<Engraving*> EngraveList;
  typedef CutList::iterator CutIt;
  typedef EngraveList::iterator EngraveIt;
  typedef CutList::const_iterator CutConstIt;
  typedef EngraveList::const_iterator EngraveConstIt;

  std::string filename;

  Document() {}
  virtual ~Document() {};

  void copy(const Document& other) {
    settings = other.settings;
    for (CutConstIt it = other.begin_cut(); it != other.end_cut(); it++) {
      this->push_back(new Cut(**it));
    }

    for (EngraveConstIt it = other.begin_engrave(); it != other.end_engrave(); it++) {
      this->push_back(new Engraving(**it));
    }
  }

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

  void push_back(Cut* cut);
  void push_back(Engraving* engraving);
  void remove(Cut* cut);
  void remove(Engraving* engraving);
  void write(std::ostream &out);

  Format guessFileFormat(const string& filename);

  bool load(const string& filename, Format docFormat = UNSPECIFIED);

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
