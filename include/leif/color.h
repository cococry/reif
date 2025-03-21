#pragma once

#include <cglm/types-struct.h>
#include <stdint.h>
#include <stdbool.h>

#define LF_NO_COLOR ((lf_color_t){0,0,0,0})
#define LF_BLACK ((lf_color_t){0,0,0,255})
#define LF_WHITE ((lf_color_t){255,255,255,255})
#define LF_RED ((lf_color_t){255,0,0,255})
#define LF_GREEN ((lf_color_t){0,255,0,255})
#define LF_BLUE ((lf_color_t){0,0,255,255})

typedef struct {
  float r, g, b, a;
} lf_color_t;

lf_color_t lf_color_from_hex(uint32_t hex);

uint32_t lf_color_to_hex(lf_color_t color);

vec4s lf_color_to_zto(lf_color_t color);

lf_color_t lf_color_from_zto(vec4s zto);

bool lf_color_equal(lf_color_t a, lf_color_t b);

lf_color_t lf_color_dim(lf_color_t color, float brightness_percent);
