#include "Brm90130.hpp"
#include "encoder/RdEncoder.hpp"
#include "config/CutSettings.hpp"
#include "../cut/operations/Chop.hpp"

#define MAGIC 0x33

typedef EngraveSettings E_SET;
typedef DocumentSettings D_SET;
typedef CutSettings C_SET;


inline char scramble(char p) {
  char a = p & 0x7E | p >> 7 & 0x01 | p << 7 & 0x80;
  char b = a ^ MAGIC;
  char s = (b + 1) & 0xFF;
  return s; 
}

class scrabled_streambuf : public std::streambuf {
    public:
      scrabled_streambuf(std::streambuf &real)
        : realBuffer(real) {}

    protected:
      int overflow(int ch) {
        int sch = scramble(ch);
        realBuffer.sputc(sch);
        return sch;
      }

    private:
        std::streambuf &realBuffer;
};


void Brm90130::write(const Document &doc, std::ostream &out) {
    bool enableEngraving = doc.get(D_SET::ENABLE_ENGRAVING) && !doc.engravings().empty();
    bool enableCut = doc.get(D_SET::ENABLE_CUT) && !doc.cuts().empty();
 
    D_SET::AirAssist airAssist = doc.get(D_SET::AIR_ASSIST);

    auto buf = out.rdbuf();
    scrabled_streambuf scrambledbuf(*buf); 
    std::ostream sout(&scrambledbuf);

    RdEncoder::writeHeader(sout);

    // TODO: Units and Stuff
    RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMin, 0, 0);
    RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMax, 1300 * 1000, 900 * 1000);

    if (airAssist == D_SET::AirAssist::GLOBAL) {
      RdEncoder::writeDeviceFlags(sout, RdEncoder::AirBlower);
    }

    if (enableEngraving) {
        if (airAssist == D_SET::AirAssist::RASTER_ONLY) {
          RdEncoder::writeDeviceFlags(sout, RdEncoder::AirBlower);
        }

        // TODO: Implement Engraving

        if (airAssist == D_SET::AirAssist::RASTER_ONLY) {
          RdEncoder::writeDeviceFlags(sout, RdEncoder::NoDevices);
        }
    }

    if (enableCut) {
      if (airAssist == D_SET::AirAssist::CUT_ONLY) {
          RdEncoder::writeDeviceFlags(sout, RdEncoder::AirBlower);
      }

      for(CutPtr cutp : doc.cuts()) {
        Cut& cut = *cutp;

        int power = cut.get(C_SET::CPOWER);
        int speed = cut.get(C_SET::CSPEED);

        // Chopping to the exact length where we should switch to relative cuts
        // Route chopped;
        // chop(cut, chopped, 8191, 8191);

        RdEncoder::encodeCut(sout, chopped, power, speed);
      }

      if (airAssist == D_SET::AirAssist::CUT_ONLY) {
        RdEncoder::writeDeviceFlags(sout, RdEncoder::NoDevices);
      }
    }

    RdEncoder::writeFooter(sout);
    sout.flush();
}
