#include "RdEncoder.hpp"
#include <bits/stdint-uintn.h>
#include <cmath>
#include <math.h>

#define ABSCOORD_LEN 5
#define RELCOORD_LEN 2
#define POWERVAL_LEN 2
#define SPEEDVAL_LEN 5
#define COLORVAL_LEN 4

#define MAX_RELCOORD_VAL_MM 8.192
#define MAX_POWER_VAL 16383
//FIXME we need to find out the maximum speed of the machine to scale the speed correctly
#define MAX_SPEED_VAL 300

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
#define SET_LAYER_COLOR "\xCA\x06"
#define SET_LAYER_MAX "\xCA\x22"

#define SET_BOUNDING_COORDS '\xE7'



void RdEncoder::encodeLayer(std::ostream &out, Route& encodee, double powerPercent, double speedPercent) {
  uint32_t power = round(MAX_POWER_VAL * (powerPercent / 100.));
  uint32_t speed = round(MAX_SPEED_VAL * (speedPercent / 100.));
  writeSetSpeedLayer(out, 0, speed);
  writeSetPowerLayer(out, Laser::LaserOneLayer | Power::PowerMin, 0, power);
  writeSetPowerLayer(out, Laser::LaserTwoLayer | Power::PowerMin, 0, power);
  writeSetPowerLayer(out, Laser::LaserOneLayer | Power::PowerMax, 0, power);
  writeSetPowerLayer(out, Laser::LaserTwoLayer | Power::PowerMax, 0, power);
  writeSetLayerColor(out, 0, 255, 0, 0);
  writeSetMaximumLayer(out, 0);
  writeDeviceFlags(out, RdEncoder::NoDevices);

  writeSetLayer(out, 0);
  writeDeviceFlags(out, RdEncoder::Unknown1);
  writeDeviceFlags(out, RdEncoder::Unknown1 | RdEncoder::Unknown2 | RdEncoder::AirBlower);

  writeSetSpeed(out, speed);

  // shrug
  out << "\xC6\x12";
  writeInt(out, 0, 5);

  out << "\xC6\x13";
  writeInt(out, 0, 5);

  writeSetPower(out, Laser::LaserOne | Power::PowerMin, power);
  writeSetPower(out, Laser::LaserTwo | Power::PowerMin, power);
  writeSetPower(out, Laser::LaserOne | Power::PowerMax, power);
  writeSetPower(out, Laser::LaserTwo | Power::PowerMax, power);

  out << "\xCA\x03\x03";
  out << "\xCA\x10";
  out << '\x00';

  SegmentPtr lastSegPtr = nullptr;
  for (const SegmentPtr segPtr : segments(encodee)) {
    if (lastSegPtr == nullptr || !segPtr->connectsTo(*lastSegPtr)) {
      auto firstPair = pointAsIntPair(segPtr->first);
      writeMoveAbsolute(out, BRM90130_BED_WIDTH - firstPair.first, firstPair.second);
    }

    Point relPoint = segPtr->second - segPtr->first;

    if (std::fabs(relPoint.x) > MAX_RELCOORD_VAL_MM || std::fabs(relPoint.y) > MAX_RELCOORD_VAL_MM) {
      auto secondPair = pointAsIntPair(segPtr->second);
      writeCutAbsolute(out, BRM90130_BED_WIDTH - secondPair.first, secondPair.second);
    } else {
      auto relPair = pointAsIntPair(relPoint);
      writeCutRelative(out, BRM90130_BED_WIDTH - relPair.first, relPair.second);
    }

    lastSegPtr = segPtr;
  }
}

std::vector<char> RdEncoder::encodeInt(int64_t integer, int64_t length) {
  std::vector<char> bytes(length);

  for(int i=0; i<length; i++) {
      bytes[length-i-1] = (integer >> (i * 7)) & '\x7F';
  }

  return bytes;
}


std::pair<int32_t, int32_t> RdEncoder::pointAsIntPair(Point p) {
  return {round(p.x), round(p.y)};
}


void RdEncoder::writeHeader(std::ostream &out) {
  // unknown header data(?)
  out << '\xd8';
  out << '\x12';
  out << '\xd8';
  out << '\x00';

  // feeding, apparently 
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
  out.write(enc.data(), enc.size());
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

void RdEncoder::writeCutRelative(std::ostream &out, int32_t x, int32_t y) {
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


void RdEncoder::writeSetPower(std::ostream &out, char selectorByte, int16_t power) {
  out << SET_POWER;
  out << selectorByte;
  writeInt(out, power, POWERVAL_LEN);
}

void RdEncoder::writeSetPowerLayer(std::ostream &out, char selectorByte, uint8_t layer, int16_t power) {
  out << SET_POWER;
  out << selectorByte;
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
  writeInt(out, b << 16 | g << 8 | r, 5);
}

void RdEncoder::writeSetBoundingCoords(std::ostream &out, uint8_t selectorByte, int32_t x, int32_t y) {
  out << SET_BOUNDING_COORDS;
  out << selectorByte;
  writeInt(out, x, ABSCOORD_LEN);
  writeInt(out, y, ABSCOORD_LEN);
}

