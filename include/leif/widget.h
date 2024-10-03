#pragma once

#include <stdint.h>

#include "event.h"
#include "util.h"
#include "color.h"

typedef enum {
  WidgetTypeUndefined = 0,
  WidgetTypeDiv,
  WidgetTypeButton,
} lf_widget_type_t;

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

struct lf_widget_t {
  lf_widget_t* parent;
  lf_widget_t** childs;
  uint32_t num_childs, cap_childs;

  lf_container_t container;

  lf_widget_props_t props;

  lf_widget_render_cb render;
  lf_widget_handle_event_cb handle_event;
  lf_widget_shape_cb shape;
  
  void* data;

  bool visible, needs_rerender, rendered;

  lf_widget_type_t type;
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

bool lf_widget_handle_event(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_event_t event);

void lf_widget_shape(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

float lf_widget_width(lf_widget_t* widget);

float lf_widget_height(lf_widget_t* widget);

int32_t lf_widget_destroy(lf_widget_t* widget);

void lf_widget_set_property(
    lf_widget_t* widget, 
    lf_widget_property_t prop, 
    void* data);

void lf_widget_reshape(
    lf_ui_state_t* ui, 
    lf_widget_t* widget);

void lf_widget_set_padding(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float padding);
