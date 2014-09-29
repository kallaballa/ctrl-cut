#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include "util/PJL.hpp"
#include "engrave/Engrave.hpp"
#include "cut/Cut.hpp"
#include "config/DocumentSettings.hpp"

#include "FileParser.hpp"
#include "util/Util.hpp"

#include <libgen.h>
#include <stdlib.h>
#include <list>
#include <map>
#include <string>
#include "stdint.h"
#include <iostream>

#include <cups/cups.h>
#include <cups/file.h>
#include <boost/thread.hpp>
#include <boost/format.hpp>

using std::string;

class Document{
public:
  enum Format {
    UNSPECIFIED,
    POSTSCRIPT,
    SVG,
    VECTOR,
    PBM,
    CTRLCUT
  };

  typedef std::list<CutPtr> CutList;
  typedef std::list<Engraving*> EngraveList;
  typedef CutList::iterator CutIt;
  typedef EngraveList::iterator EngraveIt;
  typedef CutList::const_iterator CutConstIt;
  typedef EngraveList::const_iterator EngraveConstIt;

  std::string filename;

  Document() {}
  Document(const Document& other) {
    docSettings = other.docSettings;
    const CutList& c = other.cuts();
    for(CutConstIt it = c.begin(); it != c.end(); ++it) {
      newCut() = *it;
    }

    const EngraveList& e = other.engravings();
    for(EngraveConstIt it = e.begin(); it != e.end(); ++it) {
      newEngraving() = **it;
    }
  }

  virtual ~Document() {};

  CutPtr newCut() {
    CutPtr cut(new Cut(settings()));
    this->push_back(cut);
    return cut;
  }

  Engraving& newEngraving() {
    //FIXME memory leak
    Engraving* eng = new Engraving(settings());
    this->push_back(eng);
    return *eng;
  }

  void copy(const Document& other) {
    this->docSettings = other.docSettings;
    BOOST_FOREACH(const CutPtr cut, this->cuts()) {
      this->push_back(CutPtr(new Cut(*cut.get())));
    }
    BOOST_FOREACH(const Engraving* engrave, this->engravings()) {
      this->push_back(new Engraving(*engrave));
    }
  }

  const CutList& cuts() const {
    return this->cutList;
  }

  const EngraveList& engravings() const {
    return this->engraveList;
  }

  DocumentSettings& settings() {
    return this->docSettings;
  };

  void optimize();

  void push_back(CutPtr cut);
  void push_back(Engraving* engraving);
  void remove(CutPtr cut);
  void remove(Engraving* engraving);

  Format guessFileFormat(const string& filename);
  std::pair<CutList, EngraveList> load(const string& filename, Format docFormat = UNSPECIFIED);

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    docSettings.put(key,value);
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) const {
    return docSettings.get(key);
  }

  void clear() {
    this->cutList.clear();
    this->engraveList.clear();
  }

  void toJson(std::ostream& os);
private:
  DocumentSettings docSettings;
  CutList cutList;
  EngraveList engraveList;
};

#endif /* DOCUMENT_H_ */
