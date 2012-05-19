#ifndef TESTPLANAREMBEDDING_H_
#define TESTPLANAREMBEDDING_H_

#include "cut/model/CutModel.hpp"
#include "cut/graph/Traverse.hpp"
#include "cut/geom/Geometry.hpp"
#include <boost/graph/properties.hpp>

class TestPanarEmbedding {
  static void test() {
    CutModel planar;
/* FIXME
    CutSettings settings(0,0,0);
    planar.createSegment(0, 0, 1, 0, settings);
    planar.createSegment(1, 0, 1, 1, settings);
    planar.createSegment(1, 1, 0, 1, settings);
    planar.createSegment(0, 1, 0, 0, settings);*/
  /*
    StringList strings;
    make_linestrings(strings, planar.begin(), planar.end());
    dump_linestrings(std::cerr, strings.begin(), strings.end());
  */
    SegmentGraph graph;
    create_planar_graph(graph, planar.begin(), planar.end());
    std::cerr << boost::get(vertex_index, graph)[3] << std::endl;
    assert(boyer_myrvold_planarity_test(boyer_myrvold_params::graph = graph));
  }
};

#endif
