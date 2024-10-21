#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/util.h"
#include "../include/leif/widgets/div.h"
#include <cglm/types.h>
#include <time.h>

static vec2s size_of_childs(lf_widget_t* widget);
static void adjust_widget_size(lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h);
static vec2s effective_widget_size(lf_widget_t* widget);

vec2s
size_of_childs(lf_widget_t* widget) {
  float child_w = widget->container.size.x;
  if((widget->type == WidgetTypeDiv) && 
    (lf_div_has_flag((lf_div_t*)widget, DivAdjustCenterHorizontal))
  ) {
    child_w = 0.0f;
    for(uint32_t i = 0; i < widget->num_childs; i++) {
      lf_widget_t* child = widget->childs[i];
      child_w += effective_widget_size(child).x;
    }
    child_w += widget->props.padding_left + widget->props.padding_right;
  }

  float child_h = widget->container.size.y;
  if((widget->type == WidgetTypeDiv) && 
    (lf_div_has_flag((lf_div_t*)widget, DivAdjustCenterVertical))
  ) {
    child_h = 0.0f;
    for(uint32_t i = 0; i < widget->num_childs; i++) {
      lf_widget_t* child = widget->childs[i];
      child_h += effective_widget_size(child).y;
    }
    child_h += widget->props.padding_top + widget->props.padding_bottom;
  }

  return (vec2s){
    .x = child_w,
    .y = child_h
  };
}

void 
adjust_widget_size(lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h) {
  bool fixed_w = false, fixed_h = false;
  if(widget->type == WidgetTypeDiv) {
    lf_div_t* div = (lf_div_t*)widget;
    if(div->fixed_width != -1.0f) { 
      fixed_w = true;
    }
    if(div->fixed_height != -1.0f)
      fixed_h = true;
  }

  if(widget->parent && !fixed_w) {
    widget->container.size.x = widget->parent->container.size.x - 
    ((widget->props.margin_right * 2)) - 
    ((widget->parent->props.padding_right * 2)); 
  }

  if(o_fixed_w)
    *o_fixed_w = fixed_w;
  if(o_fixed_h)
    *o_fixed_h = fixed_h;
}

vec2s 
effective_widget_size(lf_widget_t* widget) {
  return (vec2s){
    .x = lf_widget_width(widget) + 
    widget->props.margin_right + widget->props.margin_left,
    .y = lf_widget_height(widget) + 
    widget->props.margin_top + widget->props.margin_bottom
    };
}


void
lf_layout_vertical(lf_widget_t* widget) {
  bool fixed_h;
  adjust_widget_size(widget, NULL, &fixed_h);

  vec2s offset = (vec2s){
    .x = 0, 
    .y = ((widget->container.size.y - size_of_childs(widget).y) / 2.0f)
  };

  float y_before = widget->container.pos.y + offset.y;

  float y_ptr = y_before + widget->props.padding_top;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    vec2s effictive = effective_widget_size(child);

    if(widget->type == WidgetTypeDiv && lf_div_has_flag(((lf_div_t*)widget), DivAdjustCenterHorizontal))
      offset.x = ((widget->container.size.x - effictive.x) / 2.0f) - (widget->props.padding_left);

    child->container.pos.x = widget->container.pos.x + offset.x + widget->props.padding_left + child->props.margin_left;
    child->container.pos.y = y_ptr + child->props.margin_top; 

    y_ptr += effictive.y; 
  }

  if(!fixed_h) {
    float height = y_ptr - y_before - widget->props.padding_top;
    widget->container.size.y = height; 
  }
}

void 
lf_layout_horizontal(lf_widget_t* widget) {
  bool fixed_h;
  adjust_widget_size(widget, NULL, &fixed_h);

  vec2s offset = (vec2s){
    .x = ((widget->container.size.x - size_of_childs(widget).x) / 2.0f),
    .y = 0, 
  };

  float x_before = widget->container.pos.x + offset.x;

  float x_ptr = x_before + widget->props.padding_left;
  float highest_widget = 0.0f;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    vec2s effictive = effective_widget_size(child);

    if(widget->type == WidgetTypeDiv && lf_div_has_flag(((lf_div_t*)widget), DivAdjustCenterVertical))
      offset.y = ((widget->container.size.y - effictive.y) / 2.0f);

    child->container.pos.y = widget->container.pos.y + offset.y + widget->props.padding_top + child->props.margin_top;
    child->container.pos.x = x_ptr + child->props.margin_left; 

    if(effictive.y > highest_widget)
      highest_widget = effictive.y; 

    x_ptr += effictive.x; 
  }

  if(!fixed_h)
    widget->container.size.y = highest_widget; 
}

void 
lf_layout_center(lf_widget_t* widget) {
  (void)widget;
}
