#include "RdEncoder.hpp"

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

  writeSetPower(out, Laser::LaserOne | Power::PowerMin, power);
  writeSetPower(out, Laser::LaserOne | Power::PowerMax, power);
  writeSetSpeed(out, speed);

  SegmentPtr lastSegPtr = nullptr; 
  for(const SegmentPtr segPtr : segments(encodee)) {
    if (lastSegPtr == nullptr || !lastSegPtr->connectsTo(*segPtr)) {
      writeMoveAbsolute(out, segPtr->first.x, segPtr->first.y);
      writeCutAbsolute(out, segPtr->second.x, segPtr->second.y);
    } else {
      writeCutAbsolute(out, segPtr->second.x, segPtr->second.y);
    } 
  }

}

std::vector<char> RdEncoder::encodeInt(int64_t integer, int64_t length) {
  std::vector<char> bytes(length);

  for(int i=0; i<length; i++) {
      bytes.push_back((integer >> (i * 7)) & 0x7F);
  }

  return bytes;
}

void RdEncoder::writeInstructionAbsolute(std::ostream &out, uint8_t instruction, int32_t x, int32_t y) {
  out << instruction;
  auto encX = encodeInt(x, ABSCOORD_LEN);
  auto encY = encodeInt(y, ABSCOORD_LEN);
  out.write(&encX[0], encX.size());
  out.write(&encY[0], encY.size());
}

void RdEncoder::writeInstructionRelative(std::ostream &out, uint8_t instruction, int16_t x, int16_t y) {
  out << instruction;
  auto encX = encodeInt(x, RELCOORD_LEN);
  auto encY = encodeInt(y, RELCOORD_LEN);
  out.write(&encX[0], encX.size());
  out.write(&encY[0], encY.size());

}

void RdEncoder::writeInstructionSingleRelative(std::ostream &out, uint8_t instruction, int16_t c) {
  out << instruction;
  auto encC = encodeInt(c, RELCOORD_LEN);
  out.write(&encC[0], encC.size());
}


void RdEncoder::writeMoveAbsolute(std::ostream &out, int32_t x, int32_t y) {
  writeInstructionAbsolute(out, MOVE_ABS, x, y);
}

void RdEncoder::writeMoveRelative(std::ostream &out, int16_t x, int16_t y) {
  writeInstructionRelative(out, MOVE_REL, x, y);
}

void RdEncoder::writeMoveRelativeX(std::ostream &out, int16_t x) {
  writeInstructionSingleRelative(out, MOVE_REL_X, x);
}

void RdEncoder::writeMoveRelativeY(std::ostream &out, int16_t y) {
  writeInstructionSingleRelative(out, MOVE_REL_Y, y);
}


void RdEncoder::writeCutAbsolute(std::ostream &out, int32_t x, int32_t y) {
  writeInstructionAbsolute(out, CUT_ABS, x, y);
}

void RdEncoder::writeCutRelative(std::ostream &out, int16_t x, int16_t y) {
  writeInstructionRelative(out, CUT_REL, x, y);
}

void RdEncoder::writeCutRelativeX(std::ostream &out, int16_t x) {
  writeInstructionSingleRelative(out, CUT_REL_X, x);
}

void RdEncoder::writeCutRelativeY(std::ostream &out, int16_t y) {
  writeInstructionSingleRelative(out, CUT_REL_Y, y);
}


void RdEncoder::writeSetPower(std::ostream &out, uint8_t selectorByte, int16_t power) {
  out << SET_POWER;
  out << selectorByte;
  auto encPower = encodeInt(power, POWERVAL_LEN);
  out.write(&encPower[0], encPower.size());
}

void RdEncoder::writeSetPowerLayer(std::ostream &out, uint8_t selectorByte, uint8_t layer, int16_t power) {
  out << SET_POWER;
  out << selectorByte + SET_POWER_LAYER_LSOFFS;
  out << layer;
  auto encPower = encodeInt(power, POWERVAL_LEN);
  out.write(&encPower[0], encPower.size());
}


void RdEncoder::writeSetSpeed(std::ostream &out, int32_t speed) {
  out << SET_SPEED_GLOBAL;
  auto encSpeed = encodeInt(speed, SPEEDVAL_LEN);
  out.write(&encSpeed[0], encSpeed.size());
}

void RdEncoder::writeSetSpeedLayer(std::ostream &out, uint8_t layer, int32_t speed) {
  out << SET_SPEED_LAYER;
  out << layer; 
  auto encSpeed = encodeInt(speed, SPEEDVAL_LEN);
  out.write(&encSpeed[0], encSpeed.size());
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
  auto encX = encodeInt(x, ABSCOORD_LEN);
  auto encY = encodeInt(y, ABSCOORD_LEN);
  out.write(&encX[0], encX.size());
  out.write(&encY[0], encY.size());
}

