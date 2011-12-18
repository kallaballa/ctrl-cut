#include "Canvas.h"

Canvas::Canvas(dim bedWidth, dim bedHeight, dim screenWidth, dim screenHeight,
    BoundingBox* clip) : bedWidth(bedWidth), bedHeight(bedHeight),
      screenWidth(screenWidth), screenHeight(screenHeight), clip(clip), scale(1) {
  if (clip != NULL) {
    bedWidth = clip->min(bedWidth, clip->lr.x - clip->ul.x);
    bedWidth = clip->min(bedWidth, clip->lr.y - clip->ul.y);
  }
  intensity[0] = 0;

  double scale_x = (double) screenWidth / (double) bedWidth;
  double scale_y = (double) screenHeight / (double) bedHeight;

  scale = std::min(scale_x, scale_y);
}
