
#ifndef PCL_RENDERER_H_
#define PCL_RENDERER_H_

#include "engrave/Engrave.hpp"

#include <map>

class PclEncoder {
public:
	static void encode(std::ostream &out, const EngravingPtr raster);
};

#endif /* PCL_RENDERER_H_ */
