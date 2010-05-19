#include "Filter.h"
#include "RasterPass.h"
#include "VectorPass.h"

bool RasterFilter::supports(LaserPass *p)
{
  return (dynamic_cast<RasterPass*>(p) != NULL);
}

bool VectorFilter::supports(LaserPass *p)
{
  return (dynamic_cast<VectorPass*>(p) != NULL);
}

