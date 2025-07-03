#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef void (*lf_button_func_t)(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

typedef struct {
  lf_widget_t base;

  lf_button_func_t on_click;
  lf_button_func_t on_enter, on_leave;

  bool _hovered, _held;

  lf_widget_props_t hovered_props;
  lf_widget_props_t _before_hovered_props;
  bool _was_hovered;
} lf_button_t;

lf_button_t* lf_button_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);
