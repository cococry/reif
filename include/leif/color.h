#pragma once

#include <stdint.h>

#define LF_NO_COLOR ((lf_color_t){0,0,0,0})

typedef struct {
  uint8_t r, g, b, a;
} lf_color_t;

lf_color_t lf_color_from_hex(uint32_t hex);

uint32_t lf_color_to_hex(lf_color_t color);
