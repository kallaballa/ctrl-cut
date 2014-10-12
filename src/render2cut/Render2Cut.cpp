
#include "util/Logger.hpp"
#include "Document.hpp"
#include "cut/Cut.hpp"
#include "CupsGetOpt.hpp"
#include "svg/SvgWriter.hpp"
#include "cutters/EpilogLegend36Ext.hpp"

int main(int argc, char *argv[]) {
  typedef DocumentSettings DS;
  Logger::init(CC_WARNING);
  clock_t start = clock();

  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  Document doc;
  CupsOptions cupsOpts = CupsGetOpt::load_document(doc, argc, argv);
  
  SvgWriter svgW(doc, std::cout);
  svgW.write(doc, "stroke:rgb(255,0,0);stroke-width:5;");

  clock_t end = clock() - start;
  float seconds = 1.0 * end / CLOCKS_PER_SEC;
  LOG_DEBUG(seconds);

  return 0;
}
