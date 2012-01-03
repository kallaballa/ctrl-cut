
/* REFACTOR
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
}*/

