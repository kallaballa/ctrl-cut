#ifndef LASER_PASS_H_
#define LASER_PASS_H_

#include <string>
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include "Raster.h"
#include "TilePartitioner.h"
#include "Primitives.h"

using std::map;
using std::list;
using std::vector;
using std::string;
using std::ostream;

//stub
struct pass_params {
};

class LaserPass {
    pass_params params;

public:
    struct laser_config *lconf;
    LaserPass() {}
    LaserPass(pass_params params) : params(params) {}
    virtual ~LaserPass() {}
};

class VectorPass : public LaserPass {
public:
    //TODO: super inefficent string based key comparator
    JointMap joints;
    LineSegmentList lines;


    //only optionally generated, depending on filters
    PolylineVector polylines;
    OnionSkinVector skins;

    VectorPass(){};
    VectorPass(pass_params params) : LaserPass(params)    {};
    virtual ~VectorPass(){};

    Joint* addJoint(Joint* p);

    void addLine(Joint *start, Joint *end, int power);
    LineSegmentList::iterator eraseLine(LineSegmentList::iterator it_ls);

    void addOnionSkin(OnionSkin *skin);
    void load(const string &file_name);
    void write(const string &file_name);

    void serializeTo(ostream &out);
};

class RasterPass: public LaserPass {
private:
    void serializeTileTo(Tile* tile, ostream &out);
public:
	Image* sourceImage;
    list<Tile*> tiles;
    list<TilePartitioner*> partitioners;

    RasterPass(Image* sourceImage) {
    	this->sourceImage = sourceImage;
    }

    RasterPass(pass_params params) : LaserPass(params) {}
    virtual ~RasterPass() {}

    void addSourceImage(Image* img);
    void addTile(Tile* tile);
    void serializeTo(ostream &out);
};

#endif
