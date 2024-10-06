#include "../include/leif/color.h"

lf_color_t 
lf_color_from_hex(uint32_t hex) {
  lf_color_t color;
  color.r = (hex>> 16) & 0xFF;
  color.g = (hex >> 8) & 0xFF; 
  color.b = hex& 0xFF; 
  color.a = 255; 
  return color;
}

uint32_t 
lf_color_to_hex(lf_color_t color) {
  return ((uint32_t)color.r << 24) | 
  ((uint32_t)color.g << 16) | 
  ((uint32_t)color.b << 8)  | 
  ((uint32_t)color.a << 0);
}


vec4s 
lf_color_to_zto(lf_color_t color) {
  return (vec4s){
    .r = color.r / 255.0f, 
    .g = color.g / 255.0f, 
    .b = color.b / 255.0f, 
    .a = color.a / 255.0f};
}

bool 
lf_color_equal(lf_color_t a, lf_color_t b) {
  return (
    a.r == b.r &&
    a.g == b.g &&
    a.b == b.b &&
    a.a == b.a 
  );
}
