#include "../include/leif/util.h"

bool
lf_point_intersets_container(vec2s point, lf_container_t container) {
    return (point.x >= container.pos.x &&
            point.x <= container.pos.x + container.size.x &&
            point.y >= container.pos.y &&
            point.y <= container.pos.y + container.size.y);
 }

float 
lf_container_area(lf_container_t container) {
  return container.size.x * container.size.y;
}
