/*
 * EpilogCUPS - A laser cutter CUPS driver
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

#ifndef STATISTIC_H_
#define STATISTIC_H_

#include <assert.h>
#include "2D.hpp"
#include <boost/format.hpp>

enum STAT_SLOT { SLOT_RASTER, SLOT_VECTOR, SLOT_GLOBAL };
enum STAT_UNIT { UNIT_MM, UNIT_IN, UNIT_PPT };

class Slot {
public:
  double workLen;
  double moveLen;
  uint32_t penDownCnt;
  uint32_t penUpCnt;
  uint32_t segmentCnt;
  BoundingBox bbox;

  Slot(): workLen(0), moveLen(0), penDownCnt(0), penUpCnt(0) {
  }

  virtual ~Slot(){};
};

class Statistic {
private:
  uint32_t width;
  uint32_t height;
  Slot *slots;
  const double in_factor;
  const double mm_factor;
  static Statistic* instance;

public:
  static Statistic* init(uint32_t width, uint32_t height, uint16_t resolution);
  static Statistic* singleton();

  Statistic(uint32_t width, uint32_t height, uint16_t resolution) : width(width), height(height), slots(new Slot[2]),  in_factor(10 / resolution), mm_factor(25.5/ resolution){
    slots[SLOT_RASTER] = *(new Slot());
    slots[SLOT_VECTOR] = *(new Slot());
  };

  virtual ~Statistic() {};

  double distance(const Point& from, const Point& to) const {
    return hypot(abs(from.x - to.x), abs(from.y - to.y));
  }

  void announceWork(const Point& from, const Point& to, const STAT_SLOT slot) {
    slots[slot].workLen += distance(from, to);
    slots[slot].segmentCnt++;
    slots[slot].bbox.update(from);
    slots[slot].bbox.update(to);
  }

  void announceMove(const Point& from, const Point& to, const STAT_SLOT slot) {
    assert(to.x < this->width && to.y < this->height);
    slots[slot].moveLen += distance(from, to);
  }

  void announcePenDown(const STAT_SLOT slot) {
    slots[slot].penDownCnt++;
  }

  void announcePenUp(const STAT_SLOT slot) {
    slots[slot].penUpCnt++;
  }

  double convert(const double ppt, const STAT_UNIT unit) const {
    if(unit == UNIT_PPT)
      return ppt;
    else if(unit == UNIT_MM)
      return ppt * mm_factor;
    else if(unit == UNIT_IN)
      return ppt * in_factor;

    return -1; // missed a unit?
  }

  double getWorkLength(const STAT_SLOT slot, const STAT_UNIT unit=UNIT_MM) const {
    if(slot != SLOT_GLOBAL)
      return convert(slots[slot].workLen, unit);
    else
      return convert(slots[0].workLen + slots[1].workLen, unit);
  }

  double getMoveLength(const STAT_SLOT slot, const STAT_UNIT unit=UNIT_MM) const {
    if(slot != SLOT_GLOBAL)
      return convert(slots[slot].moveLen, unit);
    else
      return convert(slots[0].moveLen + slots[1].moveLen, unit);
  }

  double getTotalLength(const STAT_SLOT slot, const STAT_UNIT unit=UNIT_MM) const {
    if(slot != SLOT_GLOBAL)
      return convert(slots[slot].workLen + slots[slot].moveLen, unit);
    else
      return convert(slots[0].workLen + slots[0].moveLen + slots[1].workLen + slots[1].moveLen, unit);
  }

  uint32_t getPenUpCount(const STAT_SLOT slot) const {
    if(slot != SLOT_GLOBAL)
      return slots[slot].penUpCnt;
    else
      return slots[0].penUpCnt + slots[1].penUpCnt;
  }

  uint32_t getPenDownCount(const STAT_SLOT slot) const {
    if(slot != SLOT_GLOBAL)
      return slots[slot].penDownCnt;
    else
      return slots[0].penDownCnt + slots[1].penDownCnt;
  }

  uint32_t getSegmentCount(const STAT_SLOT slot) const {
    if(slot != SLOT_GLOBAL)
      return slots[slot].segmentCnt;
    else
      return slots[0].segmentCnt + slots[1].segmentCnt;
  }

  BoundingBox& getBoundingBox(const STAT_SLOT slot) const {
    if(slot != SLOT_GLOBAL)
      return slots[slot].bbox;
    else {
      BoundingBox& globalBBox = *(new BoundingBox());
      globalBBox += slots[0].bbox;
      globalBBox += slots[1].bbox;
      return globalBBox;
    }
  }

  void printSlot(ostream& os, const STAT_SLOT slot, const STAT_UNIT unit=UNIT_MM) const {
    string slotName;
    switch(slot) {
    case SLOT_RASTER:
      slotName = "RASTER";
      break;
    case SLOT_VECTOR:
      slotName = "VECTOR";
      break;
    case SLOT_GLOBAL:
      slotName = "GLOBAL";
      break;
    }

    os << slotName << "\t| work length=" << format("%f") % getWorkLength(slot,unit) << endl;
    os << slotName << "\t| move length=" << format("%f") % getMoveLength(slot,unit) << endl;
    os << slotName << "\t| total length=" << format("%f") % getTotalLength(slot,unit) << endl;
    os << slotName << "\t| penUp count=" << getPenUpCount(slot) << endl;
    os << slotName << "\t| penDown count=" << getPenDownCount(slot) << endl;
    os << slotName << "\t| segment count=" << getSegmentCount(slot) << endl;
    BoundingBox& bbox = getBoundingBox(slot);
    os << slotName << "\t| bounding box=" << bbox.ul.x << " " << bbox.ul.y << " " << bbox.lr.x << " " << bbox.lr.y << endl;
  }
};

Statistic* Statistic::instance = NULL;
Statistic* Statistic::init(uint32_t width, uint32_t height, uint16_t resolution) {
  assert(instance == NULL);
  return instance = new Statistic(width, height, resolution);
}

Statistic* Statistic::singleton() {
  assert(instance != NULL);
  return instance;
}
#endif /* STATISTIC_H_ */
