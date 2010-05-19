#ifndef VECTORPASS_H_
#define VECTORPASS_H_

#include "LaserPass.h"

class VectorPass : public LaserPass
{
 public:
  static VectorPass *createFromFile(const string &filename);

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

#endif
