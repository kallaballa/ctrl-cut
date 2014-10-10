
#ifndef SVGPLOT_H_
#define SVGPLOT_H_

#include "cut/geom/Route.hpp"

void find_shared_points(const Route& r, std::vector<Point>& sharedPoints);
void plot_shared_segments(const Route& r, const char* filename);
void plot_shared_points(const Route& r, const char* filename);
void plot_path_order(const Route& r, const char* filename);
void plot_segment_order(const Route& r, const char* filename);
void plot_svg(const Route& r, const string& prefix);

#endif
