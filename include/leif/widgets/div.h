#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  bool held, hovered;
  lf_container_t container;
  lf_color_t color;
} lf_scrollbar_t; 

typedef enum {
  LF_SCROLLBAR_VERTICAL = 0,
  LF_SCROLLBAR_HORIZONTAL = 1,
  LF_SCROLLBAR_COUNT = 2,
} lf_scrollbar_type_t;

typedef struct {
  lf_widget_t base;

  int32_t _column_count;

  lf_scrollbar_t scrollbars[LF_SCROLLBAR_COUNT];

  float _scroll_velocity, _last_scroll_end;

  vec2s _scrollbar_drag_start;
  vec2s _scroll_offset_start;
} lf_div_t;

lf_div_t* lf_div_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);

void lf_div_set_column_count(
    lf_div_t* div,
    uint32_t column_count);
