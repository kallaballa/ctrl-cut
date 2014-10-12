
#include "CtrlCutParser.hpp"

#include <string>
#include <fstream>
#include <vector>
#include <Magick++.h>
#include "Document.hpp"
#include "CtrlCutException.hpp"
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using std::vector;

class CtrlCutSax : public SaxParser
{
private:
  Document* doc;
  Document::CutList* newCuts;
  Document::EngraveList* newEngs;

  CutPtr currentCut;
  EngravingPtr currentEng;
  typedef DocumentSettings DS;
  typedef CutSettings CS;
  typedef EngraveSettings ES;

public:

  CtrlCutSax(Document& doc, Document::CutList& newCuts, Document::EngraveList& newEngs) : doc(&doc), newCuts(&newCuts), newEngs(&newEngs), currentCut(NULL), currentEng(NULL) {}
  virtual ~CtrlCutSax(){}

protected:
  virtual void on_characters(const Glib::ustring& characters) {
  }

  virtual void on_comment(const Glib::ustring& text){
  }

  virtual void on_cdata_block(const Glib::ustring& text) {
  }

  virtual void on_end_element(const Glib::ustring& name) {
  };

  Point parseTransform(const std::string& input) {
    boost::regex re;
    boost::cmatch matches;
    re = "translate[(]\\s*(\\d*)\\s*,\\s*(\\d*)\\s*[)]";

    if (boost::regex_match(input.c_str(), matches, re)) {
      if(matches.size() == 3) {
        Coord_t x,y;
        x = boost::lexical_cast<Coord_t>(matches[1]);
        y = boost::lexical_cast<Coord_t>(matches[2]);
        return Point(x,y);
      }
    }

    CtrlCutException::malformedDocument("Couldn't parse transform");
    return Point();
  }

  bool findValue(const AttributeList& properties, const string& name, string& value) {
    for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
      Attribute attr = *it;
      if(attr.name == name) {
        value = attr.value;
        return true;
      }
    }
    return false;
  }

  void handleImage(const AttributeList& prop) {
    string value;
    string datalink;
    Coord_t width, height;
    if(!findValue(prop, "xlink:href",datalink)) {
      CtrlCutException::malformedDocument("No image found");
    }

    if(findValue(prop, "width",value)) {
      width = boost::lexical_cast<Coord_t>(value);
    }

    if(findValue(prop, "height",value)) {
      height = boost::lexical_cast<Coord_t>(value);
    }

    const string pngbase64Sig="data:image/png;base64,";
    string pngBase64 = datalink.erase(0, pngbase64Sig.size());
    replace ( pngBase64.begin(), pngBase64.end(), ' ', '\n');

    //FIXME memory leak
    Magick::Blob pngblob;
    Magick::Blob grayblob;
    pngblob.base64(pngBase64);
    Magick::Image img(pngblob);
    img.magick( "GRAY" );
    img.write(&grayblob);

    std::cerr << "gray blob length: " << grayblob.length() << std::endl;
    char* data = new char[grayblob.length()];
    memcpy(data,grayblob.data(), grayblob.length());
    GrayscaleImage gsimg(img.columns(), img.rows(), 1, (void*) data);
    currentEng->setImage(gsimg);
  }

  void handlePolyline(const AttributeList& prop) {
    string points;

    if(findValue(prop, "points",points)) {
      typedef boost::tokenizer<boost::char_separator<char> > PointTok_t;

      boost::char_separator<char> tokenSep(" ");
      PointTok_t tok(points, tokenSep);
      string pair;
      Path path;
      for(PointTok_t::iterator it = tok.begin(); it != tok.end(); ++it) {
        vector<string> pair;
        boost::split(pair, *it, boost::is_any_of(","));
        if(pair.size() != 2)
          CtrlCutException::malformedDocument("Malformed point encountered");

        Point p;
        p.x = boost::lexical_cast<Coord_t>(pair[0]);
        p.y = boost::lexical_cast<Coord_t>(pair[1]);
        path.push_back(p);
      }
      if(currentCut == NULL)
        CtrlCutException::malformedDocument("Cut not initialized");
      currentCut->push_back(path);

    } else {
      CtrlCutException::malformedDocument("Couldn't find points");
    }
  }

  void handleSvg(const AttributeList& prop) {
    Coord_t width = -1, height = -1;
    uint32_t resolution;
    string value;

    if(findValue(prop, "width",value)) {
      width = boost::lexical_cast<Coord_t>(value);
    }

    if(findValue(prop, "height",value)) {
      height = boost::lexical_cast<Coord_t>(value);
    }

    if(findValue(prop, "resolution",value)) {
      resolution = boost::lexical_cast<uint32_t>(value);
    } else {
      CtrlCutException::malformedDocument("Resolution not found");
    }

    if(width == -1 || height == -1) {
      CtrlCutException::malformedDocument("Dimensions not found");
    }

    doc->put(DS::WIDTH, Distance(width, PX, resolution));
    doc->put(DS::HEIGHT, Distance(height, PX, resolution));
    doc->put(DS::RESOLUTION, resolution);
  }

  void handleDocument(const AttributeList& prop) {
    string value;

    if(!findValue(prop, "title",value))
      CtrlCutException::malformedDocument("Title not found");

    doc->put(DS::TITLE,value);

    if(!findValue(prop, "autofocus",value))
      CtrlCutException::malformedDocument("Autofocus not found");

    if(value == "true")
      doc->put(DS::AUTO_FOCUS,true);
    else
      doc->put(DS::AUTO_FOCUS,false);
  }

  void handleG(const AttributeList& prop) {
    string value;
    string type;
    uint32_t speed, power;
    Point pos;

    if(!findValue(prop, "ctrlcut:type",value))
      CtrlCutException::malformedDocument("Couldn't determine layer type");
    type = value;

    if(!findValue(prop, "ctrlcut:speed",value))
      CtrlCutException::malformedDocument("Couldn't find speed");
    speed = boost::lexical_cast<uint32_t>(value);

    if(!findValue(prop, "ctrlcut:power",value))
      CtrlCutException::malformedDocument("Couldn't find power");
    power = boost::lexical_cast<uint32_t>(value);

    if(!findValue(prop, "transform",value))
      CtrlCutException::malformedDocument("Couldn't find transform");
    pos = parseTransform(value);

    if(type == "engraving") {
      uint32_t dir;
      uint32_t dither;

      if(!findValue(prop, "ctrlcut:direction",value))
        CtrlCutException::malformedDocument("Couldn't find direction");
      dir = boost::lexical_cast<uint32_t>(value);

      if(!findValue(prop, "ctrlcut:dithering",value))
        CtrlCutException::malformedDocument("Couldn't find dithering");
      dither = boost::lexical_cast<uint32_t>(value);

      currentEng = doc->newEngraving();
      newEngs->push_back(currentEng);
      currentEng->put(ES::EPOWER, power);
      currentEng->put(ES::ESPEED, speed);
      currentEng->put(ES::EPOS, pos);

      switch(dir) {
      case 0:
        currentEng->put(ES::DIRECTION, ES::TOPDOWN);
        break;
      case 1:
        currentEng->put(ES::DIRECTION, ES::BOTTOMUP);
        break;
      }

      switch(dither) {
      case 0:
        currentEng->put(ES::DITHERING, ES::DEFAULT_DITHERING);
        break;
      case 1:
        currentEng->put(ES::DITHERING, ES::BAYER);
        break;
      case 2:
        currentEng->put(ES::DITHERING, ES::FLOYD_STEINBERG);
        break;
      case 3:
        currentEng->put(ES::DITHERING, ES::JARVIS);
        break;
      case 4:
        currentEng->put(ES::DITHERING, ES::BURKE);
        break;
      case 5:
        currentEng->put(ES::DITHERING, ES::STUCKI);
        break;
      case 6:
        currentEng->put(ES::DITHERING, ES::SIERRA2);
        break;
      case 7:
        currentEng->put(ES::DITHERING, ES::SIERRA3);
        break;
      }
    } else if(type == "cut") {
      uint32_t freq;
      uint32_t sort;

      if(!findValue(prop, "ctrlcut:frequency",value))
        CtrlCutException::malformedDocument("Couldn't find direction");
      freq = boost::lexical_cast<uint32_t>(value);

      if(!findValue(prop, "ctrlcut:sort",value))
        CtrlCutException::malformedDocument("Couldn't find sorting");
      sort = boost::lexical_cast<uint32_t>(value);

      currentCut = doc->newCut();
      newCuts->push_back(currentCut);
      currentCut->put(CS::FREQUENCY, freq);
      currentCut->put(CS::CPOWER, power);
      currentCut->put(CS::CSPEED, speed);
      currentCut->put(CS::CPOS, pos);

      switch(sort) {
      case 0:
        currentCut->put(CS::SORT, CS::SIMPLE);
        break;
      case 1:
        currentCut->put(CS::SORT, CS::INNER_OUTER);
        break;
      case 2:
        currentCut->put(CS::SORT, CS::SHORTEST_PATH);
        break;
      case 3:
        currentCut->put(CS::SORT, CS::TSP);
        break;
      }
    } else {
      CtrlCutException::malformedDocument("Unknown layer type encountered");
    }
  }

  virtual void on_start_element(const Glib::ustring& name, const AttributeList& prop) {
    if (name == "svg") {
      handleSvg(prop);
    } else if (name == "ctrlcut:document") {
      handleDocument(prop);
    } else if (name == "g") {
      handleG(prop);
    } else if (name == "polyline") {
      handlePolyline(prop);
    } else if (name == "image") {
      handleImage(prop);
    }
  };
};

void CtrlCutParser::load(const string& filename, Document& doc, Document::CutList& newCuts, Document::EngraveList& newEngs) {
  CtrlCutSax parser(doc, newCuts, newEngs);
  std::ifstream in(filename.c_str());

  parser.set_substitute_entities(true);
  parser.parse_stream(in);

  in.close();
}
