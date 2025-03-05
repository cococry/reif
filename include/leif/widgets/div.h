#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  lf_widget_t base;

  int32_t _column_count;

  bool _held_scrollbar, _hovered_scrollbar;
  lf_container_t _scrollbar_container;
  lf_color_t _scrollbar_color;

  vec2s _scrollbar_drag_start;
  vec2s _scroll_offset_start;
} lf_div_t;

lf_div_t* lf_div_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);

void lf_div_set_column_count(
    lf_div_t* div,
    uint32_t column_count);
