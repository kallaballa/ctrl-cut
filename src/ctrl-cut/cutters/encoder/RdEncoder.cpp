#include "RdEncoder.hpp"
#include <math.h>

#define ABSCOORD_LEN 5
#define RELCOORD_LEN 2
#define POWERVAL_LEN 2
#define SPEEDVAL_LEN 5
#define COLORVAL_LEN 4

#define MOVE_ABS '\x88'
#define MOVE_REL '\x89'
#define MOVE_REL_X '\x8A'
#define MOVE_REL_Y '\x8B'

#define CUT_ABS '\xA8'
#define CUT_REL '\xA9'
#define CUT_REL_X '\xAA'
#define CUT_REL_Y '\xAB'

#define SET_POWER '\xC6'
#define SET_POWER_LAYER_LSOFFS '\x20'

#define SET_SPEED_GLOBAL "\xC9\x02"
#define SET_SPEED_LAYER "\xC9\x04"

#define SET_DEVICE_FLAGS "\xCA\x01"

#define SET_LAYER "\xCA\x02"
#define SET_LAYER_COLOR "\xCA\x62"
#define SET_LAYER_MAX "\xCA\x22"

#define SET_BOUNDING_COORDS '\xE7'


void RdEncoder::encode(std::ostream &out, Cut& encodee) {
  int power = encodee.get(CutSettings::CPOWER);
  int speed = encodee.get(CutSettings::CSPEED);

  writeSetPowerLayer(out, Laser::LaserOne | Power::PowerMin, 0, 0);
  writeSetPowerLayer(out, Laser::LaserOne | Power::PowerMax, 0, power);

  writeSetSpeedLayer(out, 0, speed);
  writeSetLayer(out, 0);

#if false
  SegmentPtr lastSegPtr = nullptr;
  for (const SegmentPtr segPtr : segments(encodee)) {
    if (lastSegPtr != nullptr && lastSegPtr->second != segPtr->first)
       segPtr->swap();
    Point pr = segPtr->second - segPtr->first;
    auto relPair = pointAsIntPair(pr);
    if (lastSegPtr == nullptr || !segPtr->connectsTo(*lastSegPtr)) {
      auto p1 = pointAsIntPair(segPtr->first);
      writeMoveAbsolute(out, p1.first * 1000, p1.second * 1000);
      writeCutRelative(out, relPair.first * 1000, relPair.second * 1000);
    } else {
      writeCutRelative(out, relPair.first * 1000, relPair.second * 1000);
    }
    lastSegPtr = segPtr;
  }
#else 
  SegmentPtr lastSegPtr = nullptr; 
  for(const SegmentPtr segPtr : segments(encodee)) {
    if (lastSegPtr == nullptr || !segPtr->connectsTo(*lastSegPtr)) {
      auto p1 = pointAsIntPair(segPtr->first);
      auto p2 = pointAsIntPair(segPtr->second);
      writeMoveAbsolute(out, p1.first * 1000, p1.second * 1000);
      writeCutAbsolute(out, p2.first * 1000, p2.second * 1000);
    } else {
      auto p2 = pointAsIntPair(segPtr->second);
      writeCutAbsolute(out, p2.first * 1000, p2.second * 1000);
    }
    lastSegPtr = segPtr;
  }
#endif
}

std::vector<char> RdEncoder::encodeInt(int64_t integer, int64_t length) {
  std::vector<char> bytes(length);

  for(int i=0; i<length; i++) {
      bytes[length-i-1] = (integer >> (i * 7)) & '\x7F';
  }

  return bytes;
}


std::pair<int32_t, int32_t> RdEncoder::pointAsIntPair(Point p) {
  return std::make_pair(round(p.x), round(p.y));
}


void RdEncoder::writeHeader(std::ostream &out) {
  // unknown header data(?)
  out << '\xd8';
  out << '\x12';
  out << '\xd8';
  out << '\x00';

  // similar to "SetBoundingCoords" 
  out << '\xe7';
  out << '\x06';

  writeInt(out, 0, ABSCOORD_LEN);
  writeInt(out, 0, ABSCOORD_LEN);
}

void RdEncoder::writeFooter(std::ostream &out) {
  // similar to "SetBoundingCoords" 
  out << '\xe7';
  out << '\x00';

  // similar to first header byte
  out << '\xd7';
}

void RdEncoder::writeInt(std::ostream &out, int64_t i, int64_t length) {
  std::vector<char> enc = encodeInt(i, length);
  out.write(&enc[0], enc.size());
}

void RdEncoder::writeMoveAbsolute(std::ostream &out, int32_t x, int32_t y) {
  out << MOVE_ABS;
  writeInt(out, x, ABSCOORD_LEN);
  writeInt(out, y, ABSCOORD_LEN);
}

void RdEncoder::writeMoveRelative(std::ostream &out, int16_t x, int16_t y) {
  out << MOVE_REL;
  writeInt(out, x, RELCOORD_LEN);
  writeInt(out, y, RELCOORD_LEN);
}

void RdEncoder::writeMoveRelativeX(std::ostream &out, int16_t x) {
  out << MOVE_REL_X;
  writeInt(out, x, RELCOORD_LEN);
}

void RdEncoder::writeMoveRelativeY(std::ostream &out, int16_t y) {
  out << MOVE_REL_Y;
  writeInt(out, y, RELCOORD_LEN);
}


void RdEncoder::writeCutAbsolute(std::ostream &out, int32_t x, int32_t y) {
  out << CUT_ABS;
  writeInt(out, x, ABSCOORD_LEN);
  writeInt(out, y, ABSCOORD_LEN);
}

void RdEncoder::writeCutRelative(std::ostream &out, int16_t x, int16_t y) {
  out << CUT_REL;
  writeInt(out, x, RELCOORD_LEN);
  writeInt(out, y, RELCOORD_LEN);
}

void RdEncoder::writeCutRelativeX(std::ostream &out, int16_t x) {
  out << CUT_REL_X;
  writeInt(out, x, RELCOORD_LEN);
}

void RdEncoder::writeCutRelativeY(std::ostream &out, int16_t y) {
  out << CUT_REL_Y;
  writeInt(out, y, RELCOORD_LEN);
}


void RdEncoder::writeSetPower(std::ostream &out, uint8_t selectorByte, int16_t power) {
  out << SET_POWER;
  out << selectorByte;
  writeInt(out, power, POWERVAL_LEN);
}

void RdEncoder::writeSetPowerLayer(std::ostream &out, uint8_t selectorByte, uint8_t layer, int16_t power) {
  out << SET_POWER;
  out << selectorByte + SET_POWER_LAYER_LSOFFS;
  out << layer;
  writeInt(out, power, POWERVAL_LEN);
}


void RdEncoder::writeSetSpeed(std::ostream &out, int32_t speed) {
  out << SET_SPEED_GLOBAL;
  writeInt(out, speed, SPEEDVAL_LEN);
}

void RdEncoder::writeSetSpeedLayer(std::ostream &out, uint8_t layer, int32_t speed) {
  out << SET_SPEED_LAYER;
  out << layer; 
  writeInt(out, speed, SPEEDVAL_LEN);
}


void RdEncoder::writeDeviceFlags(std::ostream &out, uint8_t deviceFlags) {
  out << SET_DEVICE_FLAGS;
  out << deviceFlags;
}


void RdEncoder::writeSetLayer(std::ostream &out, uint8_t layer) {
  out << SET_LAYER;
  out << layer;
}

void RdEncoder::writeSetMaximumLayer(std::ostream &out, uint8_t maximumLayer) {
  out << SET_LAYER_MAX;
  out << maximumLayer;
}

void RdEncoder::writeSetLayerColor(std::ostream &out, uint8_t layer, uint8_t r, uint8_t g, uint8_t b) {
  out << SET_LAYER_COLOR;
  out << layer;
  out << b;
  out << g;
  out << r;
}

void RdEncoder::writeSetBoundingCoords(std::ostream &out, uint8_t selectorByte, int32_t x, int32_t y) {
  out << SET_BOUNDING_COORDS;
  out << selectorByte;
  writeInt(out, x, ABSCOORD_LEN);
  writeInt(out, y, ABSCOORD_LEN);
}

