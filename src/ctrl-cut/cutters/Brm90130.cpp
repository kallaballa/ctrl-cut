#include "Brm90130.hpp"
#include "encoder/RdEncoder.hpp"
#include "config/CutSettings.hpp"
#include "../cut/operations/Chop.hpp"

#define MAGIC 0x33

typedef EngraveSettings E_SET;
typedef DocumentSettings D_SET;
typedef CutSettings C_SET;


inline char scramble(char p) {
  char a = (p & 0x7E) | (p >> 7 & 0x01) | (p << 7 & 0x80);
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

    //Find the global bounding box
    Box bGlobal = doc.findDocumentBox();

    // TODO: Units and Stuff
    RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMin, BRM90130_BED_WIDTH - bGlobal.min_corner.x, bGlobal.min_corner.y);
    RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMax, BRM90130_BED_WIDTH - bGlobal.max_corner.x, bGlobal.max_corner.y);
    // RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMin, 0, 0);
    // RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMax, 1300 * 1000, 900 * 1000);

    // nobody knows what this does
    sout << '\xe7';
    sout << '\x04';
    sout << '\x00';
    sout << '\x01';
    sout << '\x00';
    sout << '\x01';
    RdEncoder::writeInt(sout, 0, 5);
    RdEncoder::writeInt(sout, 0, 5);

    sout << '\xe7';
    sout << '\x05';
    sout << '\x00';
    if (enableEngraving) {
        // TODO: Implement Engraving

    }

    if (enableCut) {
      for(CutPtr cutp : doc.cuts()) {
        Cut& cut = *cutp;

        int power = cut.get(C_SET::CPOWER);
        int speed = cut.get(C_SET::CSPEED);

		//Chopping to the exact length where we should switch to relative cuts
//		Route chopped;
//		chop(cut, chopped, 100, 100);

		RdEncoder::encodeLayer(sout, cut, power, speed);
      }
    }

    RdEncoder::writeFooter(sout);
    sout.flush();
}
