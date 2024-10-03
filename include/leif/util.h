#pragma once

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <stdbool.h>

#include "color.h"

typedef struct lf_ui_state_t lf_ui_state_t;

#define LF_SCALE_CONTAINER(width, height) ((lf_container_t){  \
  .pos = (vec2s){.x = 0, .y = 0},                             \
  .size = (vec2s){.x = (width), .y = (height)}}) 

#define LF_WIDGET_SIZE_V2(widget) ((vec2s){                   \
    .x = lf_widget_width((widget)),                           \
    .y = lf_widget_height(widget)})                           \

typedef struct {
  vec2s pos, size;
} lf_container_t;

typedef void* lf_font_t;

typedef enum {
  WidgetPropertyColor = 0,

  WidgetPropertyPaddingLeft,
  WidgetPropertyPaddingRight,
  WidgetPropertyPaddingTop,
  WidgetPropertyPaddingBottom,

  WidgetPropertyCornerRadius,
  WidgetPropertyBorderWidth,
  WidgetPropertyBorderColor,
} lf_widget_property_t;

typedef struct {
  lf_color_t color;
  float padding_left, padding_right,
        padding_top, padding_bottom;


  float corner_radius, border_width;

  lf_color_t border_color;
} lf_widget_props_t;

bool lf_point_intersets_container(vec2s point, lf_container_t container);

float lf_container_area(lf_container_t container);

lf_font_t lf_load_font(lf_ui_state_t* ui, const char* filepath, uint32_t size);

lf_font_t lf_load_font_from_name(lf_ui_state_t* ui, const char* font_name, uint32_t size);
