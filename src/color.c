#include "../include/leif/color.h"
#include <stdint.h>

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
lf_color_t 
lf_color_from_zto(vec4s zto) {
  return (lf_color_t){
    .r = (uint8_t)(zto.r * 255), 
    .g = (uint8_t)(zto.g * 255), 
    .b = (uint8_t)(zto.b * 255), 
    .a = (uint8_t)(zto.a * 255)};
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

lf_color_t 
lf_color_dim(lf_color_t color, float dim) {
   if (dim < 0.0f || dim > 1.0f) {
        // Ensure dim is in the range (0, 1).
        dim = 0.9f;  // Default to a 10% dimming.
    }

    lf_color_t dimmed_color;
    dimmed_color.r = (unsigned char)(color.r * dim);
    dimmed_color.g = (unsigned char)(color.g * dim);
    dimmed_color.b = (unsigned char)(color.b * dim);
    dimmed_color.a = color.a;  // Keep the alpha the same

    return dimmed_color;
}
