#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef void (*lf_slider_func_t)(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

typedef void (*lf_slider_slide_func_t)(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float* val);

typedef struct {
  lf_widget_t base;

  lf_slider_func_t on_click;
  lf_slider_slide_func_t on_slide;
  lf_slider_func_t on_enter, on_leave;

  bool _hovered, _held;

  float* val;
  float min, max;

  lf_container_t handle;
  lf_widget_props_t handle_props, _initial_handle_props;
} lf_slider_t;

lf_slider_t* lf_slider_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent, float* val, 
    float min, float max);
