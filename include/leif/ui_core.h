#pragma once

#include "color.h"
#include "render.h"
#include "util.h"
#include "win.h"

#include <stdbool.h>

typedef struct lf_widget_t lf_widget_t;

typedef struct {
  lf_widget_props_t div_props;
  lf_widget_props_t button_props;
} lf_theme_t;

struct lf_ui_state_t {
  lf_window_t* win;

  lf_widget_t* root;

  lf_render_rect_func_t render_rect;
  lf_render_text_func_t render_text;
  lf_render_get_text_dimension_func_t render_get_text_dimension;
  lf_render_clear_color_func_t render_clear_color;
  lf_render_clear_color_area_func_t render_clear_color_area;
  lf_render_begin_func_t render_begin;
  lf_render_end_func_t render_end;
  lf_render_resize_display_func_t render_resize_display;

  void* render_state;

  lf_theme_t* theme;
};

lf_ui_state_t* lf_ui_core_init(lf_window_t* win);

lf_theme_t* lf_ui_core_default_theme();

void lf_ui_core_set_theme(lf_ui_state_t* ui, lf_theme_t* theme);

lf_ui_state_t* lf_ui_core_init_ex(
    lf_window_t* win, 
    lf_render_rect_func_t render_rect,
    lf_render_text_func_t render_text,
    lf_render_get_text_dimension_func_t render_get_text_dimension,
    lf_render_clear_color_func_t render_clear_color,
    lf_render_clear_color_area_func_t render_clear_color_area,
    lf_render_begin_func_t render_begin,
    lf_render_end_func_t render_end,
    lf_render_resize_display_func_t render_resize_display);

bool lf_ui_core_next_event(lf_ui_state_t* ui);

void lf_ui_core_begin_render(
    lf_ui_state_t* ui, 
    uint32_t render_width,
    uint32_t render_height);

void lf_ui_core_end_render(lf_ui_state_t* ui);

void lf_ui_core_terminate(lf_ui_state_t* ui);
