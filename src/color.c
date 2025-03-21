#include "../include/leif/color.h"
#include <stdint.h>
#include <stdio.h>

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
lf_color_dim(lf_color_t color, float brightness_percent) {
  // Clamp brightness percentage to a reasonable range (0% to 200%)
  if (brightness_percent < 0.0f) brightness_percent = 0.0f;
  if (brightness_percent > 200.0f) brightness_percent = 200.0f;

  lf_color_t adjusted_color;

  if (brightness_percent <= 100.0f) {
    // Dimming: Scale the color values down
    float factor = brightness_percent / 100.0f;
    adjusted_color.r = color.r * factor;
    adjusted_color.g = color.g * factor;
    adjusted_color.b = color.b * factor;
  } else {
    // Brightening: Interpolate toward 255 (white)
    float factor = (brightness_percent - 100.0f) / 100.0f;
    adjusted_color.r = color.r + (255.0f - color.r) * factor;
    adjusted_color.g = color.g + (255.0f - color.g) * factor;
    adjusted_color.b = color.b + (255.0f - color.b) * factor;
  }

  // Preserve alpha
  adjusted_color.a = color.a;

  return adjusted_color;
}
