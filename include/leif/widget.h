#pragma once

#include <stdint.h>

#include "event.h"
#include "util.h"
#include "color.h"

typedef struct lf_widget_t lf_widget_t;
typedef struct lf_ui_state_t lf_ui_state_t;

struct lf_widget_t {
  lf_widget_t* parent;
  lf_widget_t** childs;
  uint32_t num_childs, cap_childs;

  lf_container_t container;

  lf_widget_props_t props;

  void (*render)(
      lf_ui_state_t* ui,
      lf_widget_t* widget);

  void (*handle_event)(
      lf_ui_state_t* ui,
      lf_widget_t* widget, 
      lf_event_t event);
  
  void* data;

  bool visible;
};

lf_widget_t* lf_widget_create_from_parent(
    lf_widget_t* parent, 
    lf_container_t container);

lf_widget_t* lf_widget_container_create(
    lf_widget_t* parent,
    lf_container_t container, 
    lf_color_t color);

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

float lf_widget_width(lf_widget_t* widget);

float lf_widget_height(lf_widget_t* widget);

int32_t lf_widget_destroy(lf_widget_t* widget);
