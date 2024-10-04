#include "../include/leif/layout.h"
#include "../include/leif/widget.h"


void 
lf_layout_vertical(lf_widget_t* widget) {
  float y_before = widget->container.pos.y + widget->props.padding_top;
  float y_ptr = y_before;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    child->container.pos.x = widget->container.pos.x + widget->props.padding_left + 
      child->props.margin_left;
    child->container.pos.y = y_ptr + child->props.margin_top;

    y_ptr += lf_widget_height(child) + child->props.margin_bottom;
  }

  widget->container.size.y = y_ptr - y_before;
}

void 
lf_layout_horizontal(lf_widget_t* widget) {
  float x_before = widget->container.pos.x + widget->props.padding_left;
  float x_ptr = x_before;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t*  child = widget->childs[i];

    child->container.pos.x = x_ptr + widget->props.margin_left;
    child->container.pos.y = widget->container.pos.y 
      + widget->props.padding_top
      + child->props.margin_top;

    x_ptr += lf_widget_width(child) + child->props.margin_right;
  }

  widget->container.size.x = x_ptr - x_before;
}

void 
lf_layout_center(lf_widget_t* widget) {
  (void)widget;
}
