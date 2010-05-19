#ifndef VECTORPASS_H_
#define VECTORPASS_H_

#include "LaserPass.h"
#include <map>

class VectorPass : public LaserPass
{
 public:
  static VectorPass *createFromFile(const string &filename);

  void addLine(Joint *start, Joint *end, int power);
  void serializeTo(ostream &out);

  // The following are public since they are used by filters

  // only optionally generated, depending on filters
  PolylineVector polylines;
  void addOnionSkin(OnionSkin *skin);
  LineSegmentList lines;
  OnionSkinVector skins;
  Joint* addJoint(Joint* p);
  LineSegmentList::iterator eraseLine(LineSegmentList::iterator it_ls);
  
  // Print debug info
  void print(ostream &stream);

 private:
  //TODO: super inefficent string based key comparator
  typedef std::map<string, class Joint *> JointMap;
  JointMap joints;
  
  VectorPass() { }
  VectorPass(pass_params params) : LaserPass(params) { }
  virtual ~VectorPass() { }
};

#endif
