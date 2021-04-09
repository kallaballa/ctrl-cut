#ifndef BRM_90130_H_
#define BRM_90130_H_


#include <vector>
#include <fstream>
#include <cstdint>

#include "Document.hpp"


class Brm90130 {
public:
  void write(const Document &doc, std::ostream &out);
};

#endif /* BRM_90130_H_ */
