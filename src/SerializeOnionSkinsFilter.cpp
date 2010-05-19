#include "SerializeOnionSkinsFilter.h"
#include "VectorPass.h"
#include "Primitives.h"

SerializeOnionSkinsFilter::~SerializeOnionSkinsFilter() {}

void SerializeOnionSkinsFilter::filter(LaserPass *pass)
{
  std::cerr << "Serialize" << std::endl;
  VectorPass *vpass = dynamic_cast<VectorPass*>(pass);

  vpass->lines.clear();

  OnionSkin* skin;
  for (OnionSkinVector::iterator it = vpass->skins.begin(); it != vpass->skins.end(); it++) {
    skin = *it;

    for (LineSegmentList::iterator it_s = skin->segments.begin(); it_s != skin->segments.end(); it_s++)
      vpass->lines.push_back(*it_s);
  }
}
