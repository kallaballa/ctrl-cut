/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef CTRLCUTPARSER_H_
#define CTRLCUTPARSER_H_

#include <libxml++/libxml++.h>
#include <string>
#include "Document.hpp"

using std::string;
using xmlpp::SaxParser;

class CtrlCutParser {
public:
  typedef SaxParser::Attribute Attribute;
  typedef SaxParser::AttributeList AttributeList;

  CtrlCutParser() { }
  void load(const string& filename, Document& doc, Document::CutList& newCuts, Document::EngraveList& newEngs);
};

#endif /* CTRLCUTPARSER_H_ */
