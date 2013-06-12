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
#include <boost/format.hpp>

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
    this->docSettings = other.docSettings;
    BOOST_FOREACH(const Cut* cut, this->cuts()) {
      this->push_back(new Cut(*cut));
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

  void push_back(Cut* cut);
  void push_back(Engraving* engraving);
  void remove(Cut* cut);
  void remove(Engraving* engraving);

  Format guessFileFormat(const string& filename);
  bool load(const string& filename, Format docFormat = UNSPECIFIED);

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    docSettings.put(key,value);
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) const {
    return docSettings.get(key);
  }
private:
  DocumentSettings docSettings;
  CutList cutList;
  EngraveList engraveList;
};

#endif /* DOCUMENT_H_ */
