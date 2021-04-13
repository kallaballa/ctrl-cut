#ifndef HPGL_ENCODER_H_
#define HPGL_ENCODER_H_

#include <iostream>
#include <vector>
#include <utility>
#include "cut/Cut.hpp"

class RdEncoder {

public:  
  enum Laser {
    LaserOne = '\x00',
    LaserTwo = '\x20',
  };

  enum Power {
    PowerMin = '\x01',
    PowerMax = '\x02',
  };

  enum BoundingCoords {
    BoundingMin = '\x03',
    BoundingMax = '\x07',
  };
  
  enum Devices {
    NoDevices = '\x00',
    AirBlower = '\x01',
  };

  static void encode(std::ostream &out, Cut& model);

  inline static std::pair<int, int> pointAsIntPair(Point p);
  inline static std::vector<char> encodeInt(int64_t integer, int64_t length);
  inline static void writeInt(std::ostream &out, int64_t i, int64_t length);

  // used outside of translation unit therefore no explicit `inline`
  static void writeHeader(std::ostream &out);
  static void writeFooter(std::ostream &out);
  static void writeDeviceFlags(std::ostream &out, uint8_t deviceFlags);
  static void writeSetBoundingCoords(std::ostream &out, uint8_t selectorByte, int32_t x, int32_t y);

  inline static void writeMoveAbsolute(std::ostream &out, int32_t x, int32_t y);
  inline static void writeMoveRelative(std::ostream &out, int16_t x, int16_t y);
  inline static void writeMoveRelativeX(std::ostream &out, int16_t x);
  inline static void writeMoveRelativeY(std::ostream &out, int16_t y);

  inline static void writeCutAbsolute(std::ostream &out, int32_t x, int32_t y);
  inline static void writeCutRelative(std::ostream &out, int16_t x, int16_t y);
  inline static void writeCutRelativeX(std::ostream &out, int16_t x);
  inline static void writeCutRelativeY(std::ostream &out, int16_t y);

  inline static void writeSetPower(std::ostream &out, uint8_t selectorByte, int16_t power);
  inline static void writeSetPowerLayer(std::ostream &out, uint8_t selectorByte, uint8_t layer, int16_t power);

  inline static void writeSetSpeed(std::ostream &out, int32_t speed);
  inline static void writeSetSpeedLayer(std::ostream &out, uint8_t layer, int32_t speed);

  inline static void writeSetLayer(std::ostream &out, uint8_t layer);
  inline static void writeSetMaximumLayer(std::ostream &out, uint8_t maximumLayer);
  inline static void writeSetLayerColor(std::ostream &out, uint8_t layer, uint8_t r, uint8_t g, uint8_t b);

};

#endif /* HPGL_ENCODER_H_ */
