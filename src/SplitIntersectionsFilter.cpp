#include "SplitIntersectionsFilter.h"

#include "LineSegment.h"
#include "LaserJob.h"
#include "Filter.h"
#include "VectorPass.h"

#include <string>
#include <list>


SplitIntersectionsFilter::~SplitIntersectionsFilter() {
  // TODO Auto-generated destructor stub
}

void SplitIntersectionsFilter::filter(LaserPass *pass)
{
  std::cerr << "Split" << std::endl;
  VectorPass *vpass = dynamic_cast<VectorPass*>(pass);

  Joint *intersec = NULL;
  LineSegment *ls1, *ls2;

  list<LineSegment*>::iterator it_i;
  list<LineSegment*>::iterator it_j;

  size_t  numLines = vpass->lines.size();
  unsigned int  percent = numLines / 100;
  unsigned int  cntLines = 0;
  for (it_i = vpass->lines.begin(); it_i != vpass->lines.end(); it_i++) {
    if (cntLines % percent == 0) std::cerr << cntLines / percent << std::endl;
    cntLines++;
    for (it_j = vpass->lines.begin(); it_j != vpass->lines.end(); it_j++) {
      ls2 = *it_j;
      ls1 = *it_i;

      if (it_i == vpass->lines.end())
        break;

      if (ls1 == ls2)
        continue;

      if ((intersec = ls1->intersects(ls2)) != NULL) {
        intersec = vpass->addJoint(intersec);

        if (!ls1->getStart()->equals(intersec)
            && !ls1->getEnd()->equals(intersec)) {
          it_i = vpass->eraseLine(it_i);
          vpass->addLine((Joint*) ls1->getStart(), intersec,
                         ls1->getPower());
          vpass->addLine((Joint*) ls1->getEnd(), intersec,
                         ls1->getPower());
        }

        if (!ls2->getStart()->equals(intersec)
            && !ls2->getEnd()->equals(intersec)) {
          it_j = vpass->eraseLine(it_j);
          vpass->addLine((Joint*) ls2->getStart(), intersec,
                         ls2->getPower());
          vpass->addLine((Joint*) ls2->getEnd(), intersec,
                         ls2->getPower());
        }
      }
    }
  }
}
