#pragma once

#include <stdint.h>

#include "event.h"
#include "util.h"
#include "layout.h"

typedef float (*lf_animation_func_t)(float t);

typedef enum {
  WidgetTypeUndefined = 0,
  WidgetTypeRoot,
  WidgetTypeDiv,
  WidgetTypeButton,
} lf_widget_type_t;

typedef struct {
  float* target;          
  float start_value;      
  float end_value;        
  float duration;        
  float elapsed_time;     
  lf_animation_func_t easing;  
  int active; 
} lf_widget_animation_t;

typedef struct lf_widget_animation_node_t {
    lf_widget_animation_t base;
    struct lf_widget_animation_node_t* next;
} lf_widget_animation_node_t;

typedef struct lf_widget_t lf_widget_t;
typedef struct lf_ui_state_t lf_ui_state_t;

typedef void (*lf_widget_render_cb)(
      lf_ui_state_t* ui,
      lf_widget_t* widget);

typedef void (*lf_widget_handle_event_cb)(
      lf_ui_state_t* ui,
      lf_widget_t* widget,
      lf_event_t event);

typedef void (*lf_widget_shape_cb)(
      lf_ui_state_t* ui,
      lf_widget_t* widget);

typedef void (*lf_widget_animate_cb)(
      lf_ui_state_t* ui,
      lf_widget_t* widget);

struct lf_widget_t {
  lf_widget_t* parent;
  lf_widget_t** childs;
  uint32_t num_childs, cap_childs;

  lf_container_t container;

  lf_widget_props_t props;

  lf_widget_render_cb render;
  lf_widget_handle_event_cb handle_event;
  lf_widget_shape_cb shape;
  lf_widget_animate_cb animate;
  
  bool visible; 

  lf_widget_type_t type;
  lf_layout_type_t layout_type;

  uint32_t listening_for;

  lf_widget_animation_node_t* anims;
};

lf_widget_t* lf_widget_create(
    lf_widget_type_t type,
    lf_container_t fallback_container,
    lf_widget_props_t props,
    lf_widget_render_cb render,
    lf_widget_handle_event_cb handle_event,
    lf_widget_shape_cb shape);

int32_t lf_widget_add_child(
    lf_widget_t* parent, 
    lf_widget_t* child);

void lf_widget_render(
    lf_ui_state_t* ui, 
    lf_widget_t* widget);

void lf_widget_handle_event(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_event_t event);

void lf_widget_shape(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

bool lf_widget_animate(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

float lf_widget_width(lf_widget_t* widget);

float lf_widget_height(lf_widget_t* widget);

int32_t lf_widget_remove(lf_widget_t* widget);

void lf_widget_set_padding(
    lf_widget_t* widget,
    float padding);

void lf_widget_set_margin(
    lf_widget_t* widget,
    float margin);

void lf_widget_set_color(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_color_t color);

void lf_widget_set_border_color(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_color_t color);

void lf_widget_set_border_width(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float border_width);

void lf_widget_set_corner_radius(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float corner_radius);

void lf_widget_set_layout(lf_widget_t* widget, lf_layout_type_t layout);

void lf_widget_apply_layout(lf_widget_t* widget);

void lf_widget_listen_for(lf_widget_t* widget, uint32_t events);

void lf_widget_unlisten(lf_widget_t* widget, uint32_t events);

bool lf_widget_is_listening(lf_widget_t* widget, uint32_t events);

void lf_widget_set_listener(
    lf_widget_t* widget, 
    lf_widget_handle_event_cb cb, 
    uint32_t events);

void lf_widget_add_animation(
    lf_widget_t* widget,
    float *target, 
    float start_value, 
    float end_value, 
    float duration, 
    lf_animation_func_t easing);

void lf_widget_interrupt_animation(
    lf_widget_t* widget,
    float *target);
