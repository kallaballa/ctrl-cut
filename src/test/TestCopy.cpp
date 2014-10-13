/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "TestCopy.hpp"
#include "Document.hpp"
#include "config/DocumentSettings.hpp"

bool any_compare(const boost::any& a1, const boost::any& a2) {
 /*   cout << "compare " << *boost::unsafe_any_cast<void*>(&a1)
         << " with:  " << *boost::unsafe_any_cast<void*>(&a2);*/
    return (*boost::unsafe_any_cast<void*>(&a1)) ==
        (*boost::unsafe_any_cast<void*>(&a2));
}

void compareSettings(Settings& first, Settings& second) {
  assert(first.size() == second.size());
  assert(first.parent == second.parent);

  Settings::iterator it_f = first.begin();
  Settings::iterator it_s = second.begin();

  while(it_f != first.end() && it_s != second.end()) {
    assert(any_compare((*(it_f++)).second, (*(it_s++)).second));
  }
  if(first.parent != NULL)
    compareSettings(*first.parent,*second.parent);
}

void compareCut(CutModel& first, CutModel& second) {
  assert(first.size() == second.size());
  CutModel::iterator it_f = first.begin();
  CutModel::iterator it_s = second.begin();

  while(it_f != first.end() && it_s != second.end()) {
    SegmentPtr* seg_f = *(it_f++);
    SegmentPtr* seg_s = *(it_s++);
    assert(seg_f != seg_s);
    assert((*seg_f) == (*seg_s));
  }

  compareSettings(first.settings, second.settings);
}

void TestCopy::test() {
  Document doc;
  CutModel orig(doc.settings);

  OpParams settings;
  settings.power = 100;
  settings.speed = 100;
  settings.frequency = 5000;
  orig.createSegment( 0, 0, 10,0 , settings);
  orig.createSegment(10, 0, 10,10, settings);
  orig.createSegment(10,10,  0,10, settings);
  orig.createSegment( 0,10,  0,0 , settings);

  CutModel copy = orig;
//  copy = orig;
  compareCut(orig, copy);
}
