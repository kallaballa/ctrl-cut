/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/*REFACTOR
void translate(SegmentList& segments, SegmentList::iterator first, SegmentList::iterator last, const Point&  translation) {
  Segment seg;
  int32_t fx;
  int32_t fy;
  int32_t sx;
  int32_t sy;

  for(SegmentList::iterator it = first; it != last; ++it) {
    seg = *it;
    fx = seg.first[0] + translation.x;
    fy = seg.first[1] + translation.y;
    sx = seg.second[0] + translation.x;
    sy = seg.second[1] + translation.y;

    //FIXME
    segments.push_back(Segment(Point(fx,fy),Point(sx,sy), seg));
  }
}
*/
