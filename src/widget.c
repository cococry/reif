#include "../include/leif/widget.h"
#include "../include/leif/ui_core.h"

#include <string.h>

#define INIT_CHILD_CAP 4

lf_widget_t* 
lf_widget_create_from_parent(
    lf_widget_t* parent, 
    lf_container_t container) {

  lf_widget_t* widget = malloc(sizeof(*widget));
  if(!widget) {
    fprintf(stderr,
            "leif: failed to create widget from parent (memory allocation failed).\n");
    return NULL;
  }

  if(parent) {
    widget->container = (lf_container_t){
      .pos = (vec2s){
        .x = parent->container.pos.x + container.pos.x,
        .y = parent->container.pos.y + container.pos.y,
      },
      .size = container.size
    };
  } else {
    widget->container = container;
  }

  widget->childs = malloc(sizeof(lf_widget_t*) * INIT_CHILD_CAP);
  widget->cap_childs = INIT_CHILD_CAP;
  widget->num_childs = 0;

  widget->parent = parent;
  widget->visible = true;

  if(parent) {
    if(lf_widget_add_child(parent, widget) != 0) {
      fprintf(stderr, 
              "leif: failed to add child (container) to parent widget.\n");
      return NULL;
    }
  }

  return widget;
}

lf_widget_t*
lf_widget_container_create(
  lf_widget_t* parent,
  lf_container_t container, 
  lf_color_t color) {

  lf_widget_t* widget = lf_widget_create_from_parent(parent, container);
  if(!widget) {
    return NULL;
  }

  widget->props = (lf_widget_props_t){
    .color = color
  };
  widget->render = lf_render_container; 
  widget->handle_event = NULL; 

  return widget;
}

void
lf_widget_render(lf_ui_state_t* ui,  lf_widget_t* widget) {
  if(widget->render)
    widget->render(ui, widget);
  
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if(child->render)
      lf_widget_render(ui, child);
  }
}

void
lf_widget_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t event) {
  if(widget->handle_event)
    widget->handle_event(ui, widget, event);
  
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if(child->handle_event)
      lf_widget_handle_event(ui, child, event);
  }
}

int32_t
lf_widget_add_child(lf_widget_t* parent, lf_widget_t* child) {
  if(!parent || !child) return 1;

  if(parent->num_childs == parent->cap_childs) {
    parent->cap_childs *= 2;
    parent->childs = realloc(parent->childs, sizeof(lf_widget_t*) * parent->cap_childs);
    if(!parent->childs) {
      return 1;
    }
  }
  parent->childs[parent->num_childs++] = child;

  return 0;
}

int32_t 
lf_widget_destroy(lf_widget_t* widget) {
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_destroy(widget->childs[i]);
  }

  if(widget->parent && widget->parent->num_childs) {
    int32_t child_idx = -1;
    for(uint32_t i = 0; i < widget->parent->num_childs; i++) {
      lf_widget_t* child = widget->parent->childs[i];
      if(child == widget) {
        child_idx = i;
        break;
      }
    }
    for(uint32_t i = child_idx; i < widget->parent->num_childs - 1; i++) {
      widget->parent->childs[i] = widget->parent->childs[i + 1];
    }
    if(child_idx == -1) {
      fprintf(stderr, 
              "leif: cannot find widget to destroy within parent.\n");
      return 1;
    }
    widget->parent->num_childs--;
  }
  memset(widget, 0, sizeof(*widget));

  free(widget->childs);
  free(widget);
  widget = NULL;

  return 0;
}

float 
lf_widget_width(lf_widget_t* widget) {
  return widget->container.size.x + 
  widget->props.padding_left       +
  widget->props.padding_right;
}

float 
lf_widget_height(lf_widget_t* widget) {
  return widget->container.size.y + 
  widget->props.padding_top       +
  widget->props.padding_bottom;
}
