#pragma once

#include "asset_manager.h"
#include "render.h"
#include "timer.h"
#include "util.h"
#include "win.h"

#include <stdbool.h>

typedef struct lf_widget_t lf_widget_t;

typedef struct {
  lf_widget_props_t div_props;
  lf_widget_props_t button_props;
  lf_widget_props_t slider_props;
  lf_widget_props_t text_props;
  lf_widget_props_t img_props;
  lf_widget_props_t scrollbar_props;
  float scrollbar_static_size;
  lf_color_t text_color, background_color;
} lf_theme_t;

typedef struct lf_page_t lf_page_t;

typedef struct {
  lf_page_t* items;
  uint32_t cap, size;
} lf_page_list_t;

typedef struct {
  lf_timer_t* items;
  uint32_t cap, size;
} lf_timer_list_t;

typedef void (*lf_page_func_t)(
      lf_ui_state_t* ui);

typedef void (*lf_idle_delay_func_t)(
      lf_ui_state_t* ui);

struct lf_ui_state_t {
  lf_window_t win;

  lf_widget_t* root;

  lf_page_list_t pages;
  lf_timer_list_t timers;

  uint64_t crnt_page_id;

  uint32_t crnt_widget_id;

  lf_asset_manager_t* asset_manager;

  lf_render_rect_func_t render_rect;
  lf_render_text_func_t render_text;
  lf_render_text_paragraph_func_t render_paragraph;
  lf_render_get_text_dimension_func_t render_get_text_dimension;
  lf_render_get_paragraph_dimension_func_t render_get_paragraph_dimension;
  lf_render_clear_color_func_t render_clear_color;
  lf_render_clear_color_area_func_t render_clear_color_area;
  lf_render_begin_func_t render_begin;
  lf_render_end_func_t render_end;
  lf_render_resize_display_func_t render_resize_display;
  lf_render_font_create render_font_create;
  lf_render_font_create_from_face render_font_create_from_face;
  lf_render_font_destroy render_font_destroy;
  lf_render_font_get_size render_font_get_size;
  lf_render_load_texture render_load_texture;
  lf_render_delete_texture render_delete_texture;
  lf_render_texture render_texture;

  void* render_state;

  lf_theme_t* theme;

  bool running;
  bool needs_render;

  uint32_t refresh_rate;
  float _frame_duration;

  float delta_time, _last_time;

  const char* fontpath;

  lf_page_func_t _root_layout_func;
  lf_idle_delay_func_t _idle_delay_func;

  lf_ez_state_t _ez;

  uint32_t active_widget_id;
};

struct lf_page_t {
  lf_page_func_t display;
  uint64_t id;
} ;

void lf_ui_core_set_window_flags(uint32_t flags);

void lf_ui_core_set_window_hint(lf_window_hint_t hint, uint32_t value);

lf_window_t lf_ui_core_create_window(
    uint32_t width, 
    uint32_t height, 
    const char* title);

lf_ui_state_t* lf_ui_core_init(lf_window_t win);

lf_theme_t* lf_ui_core_default_theme(void);

void lf_ui_core_set_theme(lf_ui_state_t* ui, lf_theme_t* theme);

lf_ui_state_t* lf_ui_core_init_ex(
    lf_window_t win, 
    void* render_state,
    lf_render_rect_func_t render_rect,
    lf_render_text_func_t render_text,
    lf_render_text_paragraph_func_t render_paragraph,
    lf_render_get_text_dimension_func_t render_get_text_dimension,
    lf_render_get_paragraph_dimension_func_t render_get_paragraph_dimension,
    lf_render_clear_color_func_t render_clear_color,
    lf_render_clear_color_area_func_t render_clear_color_area,
    lf_render_begin_func_t render_begin,
    lf_render_end_func_t render_end,
    lf_render_resize_display_func_t render_resize_display,
    lf_render_font_create render_font_create,
    lf_render_font_create_from_face render_font_create_from_face,
    lf_render_font_destroy render_font_destory,
    lf_render_font_get_size render_font_get_size,
    lf_render_load_texture render_load_texture,
    lf_render_delete_texture render_delete_texture,
    lf_render_texture render_texture);

void lf_ui_core_next_event(lf_ui_state_t* ui);

void lf_ui_core_submit(lf_ui_state_t* ui);

void lf_ui_core_begin_render(
    lf_ui_state_t* ui, 
    lf_color_t clear_color,
    uint32_t render_width,
    uint32_t render_height,
    lf_container_t render_area);

void lf_ui_core_end_render(lf_ui_state_t* ui);

void lf_ui_core_terminate(lf_ui_state_t* ui);


void lf_ui_core_remove_all_widgets(lf_ui_state_t* ui);

void lf_ui_core_add_page(lf_ui_state_t* ui, lf_page_func_t page_func, const char* identifier);

void lf_ui_core_set_page(lf_ui_state_t* ui, const char* identifier);

void lf_ui_core_set_page_by_id(lf_ui_state_t* ui, uint64_t id);

void lf_ui_core_remove_page(lf_ui_state_t* ui, const char* identifier);

void lf_ui_core_display_current_page(lf_ui_state_t* ui);

void lf_ui_core_set_root_layout(lf_ui_state_t* ui, lf_page_func_t layout_func);

lf_timer_t* lf_ui_core_start_timer(lf_ui_state_t* ui, float duration, lf_timer_finish_func_t finish_cb);

lf_timer_t* lf_ui_core_start_timer_looped(lf_ui_state_t* ui, float duration, lf_timer_finish_func_t finish_cb);

void lf_ui_core_commit_entire_render(lf_ui_state_t* ui);
