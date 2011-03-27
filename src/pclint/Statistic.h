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


enum STAT_SLOT { SLOT_RASTER, SLOT_VECTOR, SLOT_GLOBAL };
enum STAT_UNIT { UNIT_MM, UNIT_IN, UNIT_PPT };

class Slot {
public:
  double workLen;
  double moveLen;
  uint32_t penDownCnt;
  uint32_t penUpCnt;
  uint32_t segmentCnt;

  Slot(): workLen(0), moveLen(0), penDownCnt(0), penUpCnt(0) {
  }

  virtual ~Slot(){};
};

class Statistic {
private:
  Slot *slots;
  const double in_factor;
  const double mm_factor;
  static Statistic* instance;

public:
  static Statistic* singleton();

  Statistic(uint16_t resolution) : slots(new Slot[3]),  in_factor(10 / resolution), mm_factor(25.5/ resolution){
    slots[SLOT_RASTER] = *(new Slot());
    slots[SLOT_VECTOR] = *(new Slot());
    slots[SLOT_GLOBAL] = *(new Slot());
  };

  virtual ~Statistic() {};

  double distance(Point& from, Point& to) {
    return hypot(abs(from.x - to.x), abs(from.y - to.y));
  }

  Slot getSlot(STAT_SLOT slot){
    return slots[slot];
  }

  void announceWork(Point& from, Point& to, STAT_SLOT slot) {
    slots[slot].workLen += distance(from, to);
    slots[slot].segmentCnt++;
  }

  void announceMove(Point& from, Point& to, STAT_SLOT slot) {
    slots[slot].moveLen += distance(from, to);
  }

  void announcePenDown(STAT_SLOT slot) {
    slots[slot].penDownCnt++;
  }

  void announcePenUp(STAT_SLOT slot) {
    slots[slot].penUpCnt++;
  }

  double convert(double ppt, STAT_UNIT unit) {
    if(unit == UNIT_PPT)
      return ppt;
    else if(unit == UNIT_MM)
      return ppt * mm_factor;
    else if(unit == UNIT_IN)
      return ppt * in_factor;

    return -1; // missed a unit?
  }

  double getWorkLength(STAT_SLOT slot, STAT_UNIT unit=UNIT_MM) {
    return convert(slots[slot].workLen, unit);
  }

  double getMoveLength(STAT_SLOT slot, STAT_UNIT unit=UNIT_MM) {
    return convert(slots[slot].moveLen, unit);
  }

  double getTotalLength(STAT_SLOT slot, STAT_UNIT unit=UNIT_MM) {
    return convert(slots[slot].workLen + slots[slot].moveLen, unit);
  }

  uint32_t getPenUpCount(STAT_SLOT slot) {
    return slots[slot].penUpCnt;
  }

  uint32_t getPenDownCount(STAT_SLOT slot) {
    return slots[slot].penUpCnt;
  }

  uint32_t getSegmentCount(STAT_SLOT slot) {
    return slots[slot].penUpCnt;
  }

  void printSlot(ostream& os, STAT_SLOT slot, STAT_UNIT unit=UNIT_MM) {
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

    os << slotName << "\t| work length: " << getWorkLength(slot,unit) << endl;
    os << slotName << "\t| move length: " << getMoveLength(slot,unit) << endl;
    os << slotName << "\t| total length: " << getTotalLength(slot,unit) << endl;
    os << slotName << "\t| penUp count: " << getPenUpCount(slot) << endl;
    os << slotName << "\t| penDown count: " << getPenDownCount(slot) << endl;
    os << slotName << "\t| segment count: " << getSegmentCount(slot) << endl;
  }
};


Statistic* Statistic::instance = NULL;
Statistic* Statistic::singleton() {
  if (instance == NULL)
    instance = new Statistic(600);

  return instance;
}
#endif /* STATISTIC_H_ */
