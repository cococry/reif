#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/util.h"
#include "../include/leif/widgets/div.h"


void
lf_layout_vertical(lf_widget_t* widget) {
  bool fixed_width = false, fixed_height = false;
  if(widget->type == WidgetTypeDiv) {
    lf_div_t* div = (lf_div_t*)widget;
    if(div->fixed_width != -1.0f) 
      fixed_width = true;
    if(div->fixed_height != -1.0f)
      fixed_height = true;
  }

  if(widget->parent && !fixed_width)
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

  if(!fixed_height) {
    float height = y_ptr - y_before - widget->props.padding_top;
    widget->container.size.y = height; 
  }
}

void 
lf_layout_horizontal(lf_widget_t* widget) {
  bool fixed_width = false, fixed_height = false;
  if(widget->type == WidgetTypeDiv) {
    lf_div_t* div = (lf_div_t*)widget;
    if(div->fixed_width != -1.0f) { 
      fixed_width = true;
    }
    if(div->fixed_height != -1.0f)
      fixed_height = true;
  }

  if(widget->parent && !fixed_width) {
    widget->container.size.x = widget->parent->container.size.x - 
    ((widget->props.margin_right * 2)) - 
    ((widget->parent->props.padding_right * 2)); 
  }

  vec2s child_size = widget->container.size;
  if((widget->type == WidgetTypeDiv) && 
    (lf_div_has_flag((lf_div_t*)widget, DivAdjustCenterHorizontal) ||
    lf_div_has_flag((lf_div_t*)widget, DivAdjustCenterVertical))
  ) {
    bool horizontal = (lf_div_has_flag((lf_div_t*)widget, DivAdjustCenterHorizontal));
    bool vertical = (lf_div_has_flag((lf_div_t*)widget, DivAdjustCenterVertical));
    if(horizontal)
      child_size.x = 0.0f;
    if(vertical)
      child_size.y = 0.0f;

    for(uint32_t i = 0; i < widget->num_childs; i++) {
      lf_widget_t* child = widget->childs[i];
      if(horizontal) {
        float effictive_width = lf_widget_width(child) + 
          child->props.margin_left + child->props.margin_right;
        child_size.x += effictive_width;
      } 
      if(vertical) {
        float effictive_height = lf_widget_height(child) + 
          child->props.margin_top + child->props.margin_bottom;
        if(child_size.y < effictive_height)
          child_size.y = effictive_height;
      }
    }
  }

  vec2s offset = (vec2s){
    .x = ((widget->container.size.x - child_size.x) / 2.0f),
    .y = ((widget->container.size.y - child_size.y) / 2.0f)
  };

  float x_before = widget->container.pos.x + offset.x;

  float x_ptr = x_before + widget->props.padding_left;
  float highest_widget = 0.0f;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    child->container.pos.y = widget->container.pos.y + offset.y + 
      widget->props.padding_top + child->props.margin_top;
    child->container.pos.x = x_ptr + child->props.margin_left; 

    float effictive_height = lf_widget_height(child) + 
      child->props.margin_top + child->props.margin_bottom;
    if(effictive_height > highest_widget)
      highest_widget = effictive_height; 

    x_ptr += lf_widget_width(child) + child->props.margin_right + child->props.margin_left; 
  }

  if(!fixed_height)
    widget->container.size.y = highest_widget; 
}

void 
lf_layout_center(lf_widget_t* widget) {
  (void)widget;
}
