#pragma once

#include <stdint.h>

#include "event.h"
#include "util.h"
#include "layout.h"
#include "animation.h"

typedef enum {
  LF_WIDGET_TYPE_UNDEFINED = 0,
  LF_WIDGET_TYPE_ROOT,
  LF_WIDGET_TYPE_DIV,
  LF_WIDGET_TYPE_BUTTON,
  LF_WIDGET_TYPE_TEXT,
  LF_WIDGET_TYPE_IMAGE,
  LF_WIDGET_TYPE_SLIDER,
  LF_WIDGET_TYPE_INPUT,
} lf_widget_type_t;

typedef struct lf_widget_t lf_widget_t;
typedef struct lf_ui_state_t lf_ui_state_t;

typedef void (*lf_widget_render_cb)(
      lf_ui_state_t* ui,
      lf_widget_t* widget);

typedef void (*lf_widget_handle_event_cb)(
      lf_ui_state_t* ui,
      lf_widget_t* widget,
      lf_event_t* event);

typedef void (*lf_widget_shape_cb)(
      lf_ui_state_t* ui,
      lf_widget_t* widget);


struct lf_widget_t {
  lf_widget_t* parent;
  lf_widget_t** childs;
  uint32_t num_childs, cap_childs;
  uint32_t id;

  void* user_data;

  lf_container_t container;

  lf_widget_props_t props, _rendered_props, _initial_props; 
  lf_widget_props_t _component_props;

  lf_widget_render_cb render;
  lf_widget_handle_event_cb handle_event;
  lf_widget_shape_cb shape, size_calc;
  
  bool visible;
  bool rendered;
  bool scrollable;

  lf_widget_type_t type;
  lf_layout_type_t layout_type;
  lf_justify_type_t justify_type;
  lf_sizing_type_t sizing_type;

  uint32_t listening_for;

  uint32_t alignment_flags;

  bool _fixed_width;
  bool _fixed_height;
  bool _changed_by_layout, _marked_for_removal, _changed_size,
       _rendered_within_comp;
  bool _needs_size_calc, _needs_shape;
  bool _positioned_absolute_x, _positioned_absolute_y;

  lf_animation_t* anims;

  float _width_percent;
  float _height_percent;
  float _abs_x_percent;
  float _abs_y_percent;
  
  vec2s _min_size, _max_size;

  const char* font_family;
  int32_t font_size;
  lf_font_style_t font_style;

  vec2s total_child_size;
  vec2s scroll_offset, rendered_pos;

  float transition_time;
  lf_animation_func_t transition_func;

  float cull_end_y, cull_start_y;
  float cull_end_x, cull_start_x;
};

lf_widget_t* lf_widget_create(
    uint32_t id,
    lf_widget_type_t type,
    lf_container_t fallback_container,
    lf_widget_props_t props,
    lf_widget_render_cb render,
    lf_widget_handle_event_cb handle_event,
    lf_widget_shape_cb shape,
    lf_widget_shape_cb calc_size);

void lf_widget_add_child(
    lf_widget_t* parent, 
    lf_widget_t* child);

void lf_widget_render(
    lf_ui_state_t* ui, 
    lf_widget_t* widget);

void lf_widget_handle_event(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_event_t* event);

void lf_widget_size_calc(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

void lf_widget_shape(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

void lf_widget_layout(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

bool lf_widget_animate(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_widget_t** o_shape);

bool lf_widget_is_animating(lf_widget_t* widget);

float lf_widget_width(lf_widget_t* widget);

float lf_widget_height(lf_widget_t* widget);

float lf_widget_width_ex(lf_widget_t* widget, lf_widget_props_t props);

float lf_widget_height_ex(lf_widget_t* widget, lf_widget_props_t props);

void lf_widget_remove(lf_widget_t* widget);

void lf_widget_remove_from_memory(lf_widget_t* widget);

void lf_widget_remove_child_from_memory(lf_widget_t* parent, uint32_t child_idx);

lf_animation_t* lf_widget_set_padding(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float padding);

lf_animation_t* lf_widget_set_margin(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float margin);

void lf_widget_set_layout(lf_widget_t* widget, lf_layout_type_t layout);

void lf_widget_apply_layout(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_widget_calc_layout_size(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_widget_listen_for(lf_widget_t* widget, uint32_t events);

void lf_widget_unlisten(lf_widget_t* widget, uint32_t events);

bool lf_widget_is_listening(lf_widget_t* widget, uint32_t events);

lf_widget_t* lf_widget_from_id(lf_ui_state_t* ui, lf_widget_t* root, uint32_t id);

void lf_widget_set_listener(
    lf_widget_t* widget, 
    lf_widget_handle_event_cb cb, 
    uint32_t events);

lf_animation_t* lf_widget_add_animation(
    lf_widget_t* widget,
    float *target, 
    float start_value, 
    float end_value, 
    float duration,
    lf_animation_func_t func);

lf_animation_t* lf_widget_add_animation_looped(
    lf_widget_t* widget,
    float *target, 
    float start_value, 
    float end_value, 
    float duration,
    lf_animation_func_t func);

lf_animation_t* lf_widget_add_keyframe_animation(
    lf_widget_t* widget,
    float *target,
    lf_animation_keyframe_t* keyframes,
    uint32_t n_keyframes);

lf_animation_t* lf_widget_add_keyframe_animation_looped(
    lf_widget_t* widget,
    float *target,
    lf_animation_keyframe_t* keyframes,
    uint32_t n_keyframes);

void lf_widget_interrupt_animation(
    lf_widget_t* widget,
    float *target);

void lf_widget_interrupt_all_animations(
    lf_widget_t* widget);

void lf_widget_submit_props(lf_widget_t* widget);

lf_animation_t* lf_widget_set_fixed_width(lf_ui_state_t* ui, lf_widget_t* widget, float width);

lf_animation_t* lf_widget_set_fixed_height(lf_ui_state_t* ui, lf_widget_t* widget, float height);

lf_animation_t* lf_widget_set_fixed_width_percent(lf_ui_state_t* ui, lf_widget_t* widget, float percent);

lf_animation_t* lf_widget_set_fixed_height_percent(lf_ui_state_t* ui, lf_widget_t* widget, float percent);

void lf_widget_set_alignment(lf_widget_t* widget, uint32_t flags);

void lf_widget_set_sizing(lf_widget_t* widget, lf_sizing_type_t sizing);

void lf_widget_apply_size_hints(lf_widget_t* widget);

void lf_widget_set_min_width(lf_widget_t* widget, float width);

void lf_widget_set_max_width(lf_widget_t* widget, float width);

void lf_widget_set_min_height(lf_widget_t* widget, float height);

void lf_widget_set_max_height(lf_widget_t* widget, float height);

void lf_widget_set_font_style(lf_ui_state_t* ui, lf_widget_t* widget, lf_font_style_t style);

void lf_widget_set_font_family(lf_ui_state_t* ui, lf_widget_t* widget, const char* font_family);

void lf_widget_set_font_size(lf_ui_state_t* ui, lf_widget_t* widget, uint32_t pixel_size); 

vec2s lf_widget_measure_children(lf_widget_t* widget, vec2s* o_max);

vec2s lf_widget_measure_children_wrapped(lf_ui_state_t* ui, lf_widget_t* widget, vec2s* o_max);

vec2s lf_widget_effective_size(lf_widget_t* widget);

void lf_widget_set_transition_props(
    lf_widget_t* widget, float transition_time,
    lf_animation_func_t transition_func);

lf_animation_t* lf_widget_set_prop(
    lf_ui_state_t* ui,
    lf_widget_t* widget, 
    float* prop, float val);

void lf_widget_set_props(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_widget_props_t props);

lf_animation_t* lf_widget_set_prop_color(
    lf_ui_state_t* ui,
    lf_widget_t* widget, 
    lf_color_t* prop, lf_color_t val);

void lf_widget_set_visible(lf_ui_state_t* ui, lf_widget_t* widget, bool visible);

void lf_widget_invalidate_size(lf_widget_t* widget);

void lf_widget_invalidate_layout(lf_widget_t* widget);

void lf_widget_invalidate_size_and_layout(lf_widget_t* widget);

lf_widget_t* lf_widget_flag_for_layout(lf_ui_state_t* ui, lf_widget_t* widget);

void lf_widget_set_pos_x(lf_widget_t* widget, float pos);

void lf_widget_set_pos_y(lf_widget_t* widget, float pos);

bool lf_widget_or_childs_changed_size(
    lf_widget_t* widget);

void lf_widget_set_pos_x_absolute(lf_widget_t* widget, float x);

void lf_widget_set_pos_y_absolute(lf_widget_t* widget, float y);

void lf_widget_set_pos_x_absolute_percent(lf_widget_t* widget, float x);

void lf_widget_set_pos_y_absolute_percent(lf_widget_t* widget, float y);

bool lf_widget_in_viewport(lf_ui_state_t* ui, lf_widget_t* widget);

bool lf_widget_props_equal(lf_widget_props_t a, lf_widget_props_t b);
