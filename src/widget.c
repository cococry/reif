#include "../include/leif/widget.h"
#include "../include/leif/ui_core.h"
#include "../include/leif/widgets/div.h"
#include <string.h>
#include <time.h>

#define INIT_CHILD_CAP 4


static void widget_resize_children(lf_widget_t* widget, uint32_t new_cap);

void 
widget_resize_children(lf_widget_t* widget, uint32_t new_cap) {
    widget->childs = (lf_widget_t**)realloc(widget->childs, new_cap * sizeof(lf_widget_t*));
    widget->cap_childs = new_cap;
}

lf_widget_t* 
lf_widget_create(
  lf_widget_type_t type,
  lf_container_t fallback_container,
  lf_widget_props_t props,
  lf_widget_render_cb render,
  lf_widget_handle_event_cb handle_event,
  lf_widget_shape_cb shape) {
  lf_widget_t* widget = (lf_widget_t*)malloc(sizeof(lf_widget_t));

  widget->parent = NULL;
  widget->childs = NULL;
  widget->cap_childs = 0;
  widget->num_childs = 0;
  widget->visible = true;
  widget->needs_rerender = true;
  widget->rendered = false;

  widget->type = type;
  widget->container = fallback_container;
  widget->props = props;
  widget->render = render;
  widget->handle_event = handle_event;
  widget->shape = shape;

  return widget;
}

void
lf_widget_render(lf_ui_state_t* ui,  lf_widget_t* widget) {
  if(widget->render) {
    widget->render(ui, widget);
    widget->rendered = true;
    widget->needs_rerender = false;
  }    
  
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_render(ui, widget->childs[i]);
  }
}

void lf_widget_shape(
    lf_ui_state_t* ui,
    lf_widget_t* widget) {
  if(widget->shape) {
    widget->shape(ui, widget);
  }    
  
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_shape(ui, widget->childs[i]);
  }
}

bool
lf_widget_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t event) {
  if(widget->handle_event)
    widget->handle_event(ui, widget, event);
  
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    if(lf_widget_handle_event(ui, widget->childs[i], event)) break;
  }

  return widget->handle_event;
}

int32_t
lf_widget_add_child(lf_widget_t* parent, lf_widget_t* child) {
  if(!parent || !child) return 1;

  if(parent->num_childs >= parent->cap_childs) {
    uint32_t new_cap = parent->cap_childs == 0 ? 2 : parent->cap_childs * 2;
    widget_resize_children(parent, new_cap);
  }
  parent->childs[parent->num_childs++] = child;
  child->parent = parent;
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
  free(widget->childs);

  return 0;
}

float 
lf_widget_width(lf_widget_t* widget) {
  return widget->container.size.x + 
  widget->props.padding_left      +
  widget->props.padding_right;
}

float 
lf_widget_height(lf_widget_t* widget) {
  return widget->container.size.y + 
  widget->props.padding_top       +
  widget->props.padding_bottom;
}

void 
lf_set_widget_property(
  lf_widget_t* widget, 
  lf_widget_property_t prop, 
  void* data) {

  switch(prop) {
    case WidgetPropertyColor:
      widget->props.color = *(lf_color_t*)data;
      break;
    case WidgetPropertyBorderColor:
      widget->props.border_color = *(lf_color_t*)data;
      break;
    case WidgetPropertyPaddingLeft:
      widget->props.padding_left = *(float*)data;
      break;
    case WidgetPropertyPaddingRight:
      widget->props.padding_right = *(float*)data;
      break;
    case WidgetPropertyPaddingTop:
      widget->props.padding_top = *(float*)data;
      break;
    case WidgetPropertyPaddingBottom:
      widget->props.padding_bottom = *(float*)data;
      break;
    case WidgetPropertyCornerRadius:
      widget->props.corner_radius = *(float*)data;
      break;
    case WidgetPropertyBorderWidth:
      widget->props.border_width = *(float*)data;
      break;
    default:
      break;
  }
}

void lf_widget_reshape(
  lf_ui_state_t* ui, 
  lf_widget_t* widget) {
  lf_widget_shape(ui, widget);
  lf_widget_t* parent = widget->parent;
  while(parent != NULL) {
    if(parent->shape) {
      lf_widget_shape(ui, parent);
    }
    parent = parent->parent;
  }
}

void lf_widget_set_padding(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float padding) {

  if(
    widget->props.padding_top == padding && 
    widget->props.padding_bottom == padding && 
    widget->props.padding_left == padding && 
    widget->props.padding_right == padding) return;

  for(lf_widget_property_t prop = WidgetPropertyPaddingLeft; prop <= WidgetPropertyPaddingBottom; prop++) {
    lf_set_widget_property(widget, prop, (void*)&padding);
  }
  lf_widget_reshape(ui, widget);
  lf_ui_core_rerender(ui);
}

void 
lf_widget_rerender(lf_widget_t* widget) {
  widget->needs_rerender = true;
}
