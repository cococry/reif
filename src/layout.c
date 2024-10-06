#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/util.h"


void
lf_layout_vertical(lf_widget_t* widget) {
  if(widget->parent)
    widget->container.size.x = widget->parent->container.size.x - 
    ((widget->props.margin_right * 3) + (widget->props.margin_left)) - 
    ((widget->parent->props.padding_right * 4)); 

  float y_before = widget->container.pos.y;

  float y_ptr = y_before + widget->props.padding_top;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    child->container.pos.x = widget->container.pos.x + 
      widget->props.padding_left + child->props.margin_left;
    child->container.pos.y = y_ptr + child->props.margin_top; 

    y_ptr += lf_widget_height(child) + child->props.margin_bottom + child->props.margin_top; 
  }

  float height = y_ptr - y_before - widget->props.padding_top;
  widget->container.size.y = height; 
}

void 
lf_layout_horizontal(lf_widget_t* widget) {
  if(widget->parent)
    widget->container.size.x = widget->parent->container.size.x - 
    ((widget->props.margin_right * 3) + (widget->props.margin_left)) - 
    ((widget->parent->props.padding_right * 4)); 

  float x_before = widget->container.pos.x;

  float x_ptr = x_before + widget->props.padding_left;
  float highest_widget = 0.0f;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    child->container.pos.y = widget->container.pos.y + 
      widget->props.padding_top + child->props.margin_top;
    child->container.pos.x = x_ptr + child->props.margin_left; 

    float effictive_height = lf_widget_height(child) + 
      child->props.margin_top + child->props.margin_bottom;
    if(effictive_height > highest_widget)
      highest_widget = effictive_height; 

    x_ptr += lf_widget_width(child) + child->props.margin_right + child->props.margin_left; 
  }

  widget->container.size.y = highest_widget; 
}

void 
lf_layout_center(lf_widget_t* widget) {
  (void)widget;
}
