#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>
#include <list>

#include "config/DocumentSettings.h"
#include "config/CutSettings.h"
#include "cut/geom/Route.h"

template<
template<typename,typename> class Tcontainer = std::list,
template<typename> class Tallocator = std::allocator
>
class CutModelImpl : public RouteImpl<Tcontainer, Tallocator> {
public:
  typedef RouteImpl<Tcontainer, Tallocator> Route_t;
  CutModelImpl(DocumentSettings& parentSettings) :
    Route_t(parentSettings)
  { }

  //shallow copy
  CutModelImpl(const CutModelImpl& other) :
    Route_t(other) {
  }

  ~CutModelImpl() {
    this->clear();
  }

  bool append(const Segment& seg) {
    Segment clipped(seg);
    this->clip(clipped);
    if(clipped.first == clipped.second)
      return false;
    return Route_t::append(clipped);
  }


  /*!
   Loads vector data from EPS/Ghostscript output
   */
  bool load(std::istream &input) {
    std::string line;
    char first;
    int power, x, y;
    int lx = 0, ly = 0;
    int mx = 0, my = 0;
    LOG_INFO_STR("Load vector data");
    int segmentCnt = 0;
    while (std::getline(input, line)) {
      first = line[0];

      if (first == 'X') { // End of output
        break;
      }

      if (isalpha(first)) {
        switch (first) {
        case 'M': // move to
          if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
            lx = x;
            ly = y;
            mx = x;
            my = y;
          }
          break;
        case 'C': // close
          if (lx != mx || ly != my) {
            segmentCnt++;
            Route::append(lx, ly, mx, my);
          }
          break;
        case 'P': // power
          if (sscanf(line.c_str() + 1, "%d", &x) == 1) {
            // FIXME: While testing, ignore the strange color-intensity-is-power convension
            //          power = x;
            power = -1;
          }
          break;
        case 'L': // line to
          if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
            segmentCnt++;
            Route::append(lx, ly, x, y);
            lx = x;
            ly = y;
          }
          break;
        }
      }
    }

    LOG_DEBUG_MSG("loaded segments", segmentCnt);

    if (this->empty()) {
      return false;
    }

    return true;
  }

  /*!
   Loads vector data from EPS/Ghostscript output from the given file
   */
  bool load(const std::string &filename) {
    std::ifstream infile(filename.c_str(), std::ios_base::in);
    return this->load(infile);
  }

  void clip(Segment& seg) {
    typedef DocumentSettings ds;
    int resolution = this->get(ds::RESOLUTION);
    double width = this->get(ds::WIDTH).in(PX, resolution);
    double height = this->get(ds::HEIGHT).in(PX, resolution);

    Segment leftBedBorder(Point(0, 0),Point(0, height-1));
    Segment bottomBedBorder(Point(0, height-1),Point(width-1, height-1));
    Segment rightBedBorder(Point(width-1, height-1),Point(width-1, 0));
    Segment topBedBorder(Point(width-1, 0),Point(0, 0));

    Point intersection;
    Segment clipped;

    if(seg.first.x < 0 || seg.second.x < 0) {
      // out of bounds;
      if(seg.first.x < 0 && seg.second.x < 0) {
        return;
      }

      if(intersects(seg, leftBedBorder, intersection) == ALIGN_INTERSECT) {
        if(seg.first.x < seg.second.x)
          seg.first = intersection;
        else
          seg.second = seg.first;
          seg.first = intersection;

        intersection = Point();
      }
    }

    if(seg.first.y < 0 || seg.second.y < 0) {
      if(seg.first.y < 0 && seg.second.y < 0) {
        return;
      }

      if(intersects(seg, topBedBorder, intersection) == ALIGN_INTERSECT) {
        if(seg.first.y < seg.second.y)
          seg.first = intersection;
        else
          seg.second = seg.first;
          seg.first = intersection;

        intersection = Point();
      }
    }

    if(greater_than(seg.first.x,width - 1) || greater_than(seg.second.x,width - 1)) {
      if(greater_than(seg.first.x, width - 1) && greater_than(seg.second.x,width - 1)) {
        return;
      }

      if(intersects(seg, rightBedBorder, intersection) == ALIGN_INTERSECT) {
        if(seg.first.x > seg.second.x)
          seg.first = intersection;
        else
          seg.second = seg.first;
          seg.first = intersection;

        intersection = Point();
      }
    }

    if(greater_than(seg.first.y, height - 1) || greater_than(seg.second.y,height - 1)) {
      if(greater_than(seg.first.y, height - 1) && greater_than(seg.second.y,height - 1)) {
        return;
      }
      if(intersects(seg, bottomBedBorder, intersection) == ALIGN_INTERSECT) {
        if(seg.first.y > seg.second.y)
          seg.first = intersection;
        else
          seg.second = seg.first;
          seg.first = intersection;
      }
    }
  }
protected:
  /* REFACTOR
  uint64_t clipped;
  uint64_t zerolength;
  */
};

typedef CutModelImpl<std::list, std::allocator> CutModel;
#endif /* CUTMODEL_H_ */
