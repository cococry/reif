#include "../include/leif/widget.h"
#include "../include/leif/ui_core.h"
#include "../include/leif/layout.h"
#include <string.h>
#include <time.h>

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

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
    // Set rendering culling box
#ifdef LF_RUNARA
  rn_set_cull_end_x(
    (RnState*)ui->render_state, 
    widget->parent->container.pos.x + lf_widget_width(widget->parent) - 
    widget->parent->props.border_width); 
  rn_set_cull_end_y(
    (RnState*)ui->render_state, 
    widget->parent->container.pos.y + lf_widget_height(widget->parent) - 
    widget->props.border_width); 
  rn_set_cull_start_x(ui->render_state,
    widget->parent->container.pos.x + widget->parent->props.border_width); 
  rn_set_cull_start_y(ui->render_state,
    widget->parent->container.pos.y + widget->parent->props.border_width); 

#endif


    widget->render(ui, widget);
  }    
  
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_render(ui, widget->childs[i]);
  }
}

void lf_widget_shape(
    lf_ui_state_t* ui,
    lf_widget_t* widget) {
  if (widget->shape != NULL) {
    widget->shape(ui, widget);
  }

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_shape(ui, widget->childs[i]);
  }
}

void
lf_widget_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t event) {
  if(widget->handle_event)
    widget->handle_event(ui, widget, event);
  
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_handle_event(ui, widget->childs[i], event);
  }
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
lf_widget_remove(lf_widget_t* widget) {
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_remove(widget->childs[i]);
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

void lf_widget_set_padding(
  lf_widget_t* widget,
  float padding) {
  if(
    widget->props.padding_top == padding && 
    widget->props.padding_bottom == padding && 
    widget->props.padding_left == padding && 
    widget->props.padding_right == padding) return;

  widget->props.padding_top = padding;
  widget->props.padding_bottom = padding;
  widget->props.padding_left = padding; 
  widget->props.padding_right = padding;
}

void lf_widget_set_margin(
    lf_widget_t* widget,
    float margin) {
  if(
    widget->props.margin_top == margin && 
    widget->props.margin_bottom == margin && 
    widget->props.margin_left == margin && 
    widget->props.margin_right == margin) return;

  widget->props.margin_top = margin;
  widget->props.margin_bottom = margin;
  widget->props.margin_left = margin; 
  widget->props.margin_right = margin;
}

void lf_widget_set_color(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_color_t color) {
  if(lf_color_equal(widget->props.color, color)) return;

  widget->props.color = color;
  lf_ui_core_make_dirty(ui, widget); 
}

void lf_widget_set_border_color(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    lf_color_t color) {
  if(lf_color_equal(widget->props.border_color, color)) return;

  widget->props.border_color = color;
  lf_ui_core_make_dirty(ui, widget); 
}

void lf_widget_set_border_width(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float border_width) {
  if(widget->props.border_width == border_width) return;

  widget->props.border_width = border_width;
  lf_ui_core_make_dirty(ui, widget); 
}

void lf_widget_set_corner_radius(
    lf_ui_state_t* ui,
    lf_widget_t* widget,
    float corner_radius) {
  if(widget->props.corner_radius == corner_radius) return;

  widget->props.corner_radius = corner_radius;
  lf_ui_core_make_dirty(ui, widget); 
}

void 
lf_widget_set_layout(lf_widget_t* widget, lf_layout_type_t layout) {
  widget->layout_type = layout;
}

void 
lf_widget_apply_layout(lf_widget_t* widget) {
  if(widget->layout_type == LayoutVertical) {
    lf_layout_vertical(widget);
  }
  if(widget->layout_type == LayoutHorizontal) {
    lf_layout_horizontal(widget);
  }
}
