#include "Brm90130.hpp"
#include "encoder/RdEncoder.hpp"
#include "config/CutSettings.hpp"

#define MAGIC 0x33

typedef EngraveSettings E_SET;
typedef DocumentSettings D_SET;
typedef CutSettings C_SET;


char scramble(char p) {
  char a = p & 0x7E | p >> 7 & 0x01 | p << 7 & 0x80;
	char b = a ^ MAGIC;
  char s = (b + 1) & 0xFF;
  return s; 
}

class scrabled_streambuf : public std::streambuf
{
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

    auto buf = out.rdbuf();
    scrabled_streambuf scrambledbuf(*buf); 
    std::ostream sout(&scrambledbuf);

    sout << '\xd8';
    sout << '\x12';
    sout << '\xd8';
    sout << '\x00';

    RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMin, 0, 0);
    RdEncoder::writeSetBoundingCoords(sout, RdEncoder::BoundingMax, 1300 * 1000, 900 * 1000);

    // TODO: Implement AirAssist configuration 
    // D_SET::AirAssist airAssist = doc.get(D_SET::AIR_ASSIST);

    if (enableEngraving) {
        // TODO: Implement Engraving
    }

    if (enableCut) {
      for(CutPtr cutp : doc.cuts()) {
        Cut& cut = *cutp;
        RdEncoder::encode(sout, cut);
      }
    }

    sout << '\xe7';
    sout << '\x00';
    sout << '\xd7';
    sout.flush();
}
