#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>

#include "color.h"

#define LF_SCALE_CONTAINER(width, height) ((lf_container_t){  \
  .pos = (vec2s){.x = 0, .y = 0},                             \
  .size = (vec2s){.x = (width), .y = (height)}}) 

typedef struct {
  vec2s pos, size;
} lf_container_t;

typedef struct {
  lf_color_t color;
  float padding_left, padding_right,
        padding_top, padding_bottom;
} lf_widget_props_t;
