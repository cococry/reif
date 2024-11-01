#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <stdbool.h>

#include "color.h"

typedef struct lf_ui_state_t lf_ui_state_t;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) (a) < (b) ? (a) : (b)

#define LF_SCALE_CONTAINER(width, height) ((lf_container_t){  \
  .pos = (vec2s){.x = 0, .y = 0},                             \
  .size = (vec2s){.x = (width), .y = (height)}}) 

#define LF_WIDGET_SIZE_V2(widget) ((vec2s){                   \
    .x = lf_widget_width((widget)),                           \
    .y = lf_widget_height(widget)})                           \

#define LF_WIDGET_CONTAINER(widget) ((lf_container_t){        \
    .pos = (widget)->container.pos,                           \
    .size = LF_WIDGET_SIZE_V2((widget))})                     \

typedef struct {
  vec2s pos, size;
} lf_container_t;

typedef void* lf_font_t;

typedef struct {
  lf_color_t color;
  lf_color_t text_color;

  float padding_left, padding_right,
        padding_top, padding_bottom;

  float margin_left, margin_right, 
        margin_top, margin_bottom;

  float corner_radius, border_width;

  lf_color_t border_color;

} lf_widget_props_t;

bool lf_point_intersets_container(vec2s point, lf_container_t container);

bool lf_container_intersets_container(lf_container_t a, lf_container_t b);

float lf_container_area(lf_container_t container);

lf_font_t lf_load_font(lf_ui_state_t* ui, const char* filepath, uint32_t size);

lf_font_t lf_load_font_from_name(lf_ui_state_t* ui, const char* font_name, uint32_t size);

void lf_font_resize(lf_ui_state_t* ui, lf_font_t font, uint32_t size);

uint32_t lf_font_get_size(lf_ui_state_t* ui, lf_font_t font);
