#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/util.h"
#include "../include/leif/widgets/div.h"
#include "../include/leif/widgets/button.h"
#include <cglm/types.h>
#include <string.h>
#include <time.h>

static vec2s size_of_childs(lf_widget_t* widget);
static void adjust_widget_size(lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h);
static vec2s effective_widget_size(lf_widget_t* widget);

vec2s
size_of_childs(lf_widget_t* widget) {
  float child_w = widget->container.size.x;
  if((widget->type == WidgetTypeDiv) && 
    (lf_alignment_flag_exists(&((lf_div_t*)widget)->flags, AlignCenterHorizontal))
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
    (lf_alignment_flag_exists(&((lf_div_t*)widget)->flags, AlignCenterVertical))
  ) {
    printf("Vertical.\n");
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
    if(div->_fixed_width != -1.0f) { 
      fixed_w = true;
    }
    if(div->_fixed_height != -1.0f)
      fixed_h = true;
  }

  if(widget->parent && !fixed_w) {
    widget->container.size.x = widget->parent->container.size.x - 
    ((widget->props.margin_right * 3)) - 
    ((widget->parent->props.padding_right * 3)); 
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
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props.padding_left = widget->childs[i]->_initial_props.padding_left;
    widget->childs[i]->props.padding_right = widget->childs[i]->_initial_props.padding_right;
    widget->childs[i]->props.padding_top = widget->childs[i]->_initial_props.padding_top;
    widget->childs[i]->props.padding_bottom = widget->childs[i]->_initial_props.padding_bottom;

  }

  vec2s offset = (vec2s){
    .x = 0, 
    .y = ((widget->container.size.y - size_of_childs(widget).y) / 2.0f)
  };

  float y_before = widget->container.pos.y + offset.y;

  float y_ptr = y_before + widget->props.padding_top * 2;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    vec2s effictive = effective_widget_size(child);

    if(widget->type == WidgetTypeDiv && lf_alignment_flag_exists(&((lf_div_t*)widget)->flags, AlignCenterHorizontal))
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
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props.padding_left = widget->childs[i]->_initial_props.padding_left;
    widget->childs[i]->props.padding_right = widget->childs[i]->_initial_props.padding_right;
    widget->childs[i]->props.padding_top = widget->childs[i]->_initial_props.padding_top;
    widget->childs[i]->props.padding_bottom = widget->childs[i]->_initial_props.padding_bottom;
   
  }

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
    
    if(widget->type == WidgetTypeDiv && lf_alignment_flag_exists(&((lf_div_t*)widget)->flags, AlignCenterVertical))
      offset.y = ((widget->container.size.y - effictive.y) / 2.0f);

    child->container.pos.y = widget->container.pos.y + offset.y +  widget->props.padding_top + child->props.margin_top;
    child->container.pos.x = x_ptr + child->props.margin_left; 

    if(effictive.y > highest_widget)
      highest_widget = effictive.y; 

    x_ptr += effictive.x; 
  }

  if(!fixed_h)
    widget->container.size.y = highest_widget; 
}

void 
lf_layout_grid(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(widget->type != WidgetTypeDiv) return;
  if(!widget->num_childs) return;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props.padding_left = widget->childs[i]->_initial_props.padding_left;
    widget->childs[i]->props.padding_right = widget->childs[i]->_initial_props.padding_right;
    widget->childs[i]->props.padding_top = widget->childs[i]->_initial_props.padding_top;
    widget->childs[i]->props.padding_bottom = widget->childs[i]->_initial_props.padding_bottom;
   
  }

  bool fixed_w, fixed_h;
  adjust_widget_size(widget, &fixed_w, &fixed_h);

  uint32_t row_i = 0;
  uint32_t n_rows = (int32_t)(widget->num_childs / ((lf_div_t*)widget)->_column_count);

  float row_heights[n_rows];
  memset(row_heights, 0, sizeof(row_heights));


  row_i = 0;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    if(i % ((lf_div_t*)widget)->_column_count == 0 && i != 0) {
      row_i++;
    }
    float widget_h = effective_widget_size(widget->childs[i]).y;
    if(widget_h > row_heights[row_i] && !widget->childs[i]->changed_by_layout) {
      row_heights[row_i] = widget_h; 
    }
  }

  float total_child_h = 0.0f;
  for(uint32_t i = 0; i < n_rows; i++) {
    total_child_h += row_heights[i];
  }
  vec2s offset = (vec2s){
    .x = 0, 
    .y = 0,
  };

  bool is_div = widget->type == WidgetTypeDiv;
  bool centered_vert = is_div && lf_alignment_flag_exists(&((lf_div_t*)widget)->flags, AlignCenterVertical);

  if(is_div && centered_vert) {
    offset.y = (widget->parent->container.size.y - total_child_h) / 2.0f;
  }

  float x_before = widget->container.pos.x + offset.x;
  float y_before = widget->container.pos.x + offset.y;

  float x_ptr = x_before + widget->props.padding_left;
  float y_ptr = y_before + widget->props.padding_top;

  row_i = 0;
  float child_w = lf_widget_width(widget) / ((lf_div_t*)widget)->_column_count;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if(i % ((lf_div_t*)widget)->_column_count == 0 && i != 0) {
      y_ptr += row_heights[row_i++];
      x_ptr = x_before + widget->props.padding_left; 
    }

    float w = (child_w - child->props.margin_left - child->props.margin_right);
    float ph = (w - child->container.size.x) / 2.0f;
    float h = (row_heights[row_i] - child->props.margin_top - child->props.margin_bottom);
    float pv = (h - child->container.size.y) / 2.0f;
    child->props.padding_left = ph; 
    child->props.padding_right = ph;
    child->props.padding_top = pv; 
    child->props.padding_bottom = pv;

    vec2s effictive = effective_widget_size(child);
    child->container.pos.x = x_ptr + child->props.margin_left; 
    child->container.pos.y = y_ptr + child->props.margin_top; 
    x_ptr += effictive.x; 
  }

  if(!fixed_w) {
    widget->container.size.x = size_of_childs(widget).x;
  }
  if(!fixed_h) {
    widget->container.size.y = size_of_childs(widget).y;
  }
}

void 
lf_layout_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(widget->type != WidgetTypeDiv) return;
  if(!widget->num_childs) return;

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props.padding_left = widget->childs[i]->_initial_props.padding_left;
    widget->childs[i]->props.padding_right = widget->childs[i]->_initial_props.padding_right;
    widget->childs[i]->props.padding_top = widget->childs[i]->_initial_props.padding_top;
    widget->childs[i]->props.padding_bottom = widget->childs[i]->_initial_props.padding_bottom;
  }

  bool fixed_w, fixed_h;
  adjust_widget_size(widget, &fixed_w, &fixed_h);

  uint32_t row_i = 0;
  uint32_t n_rows = (int32_t)(widget->num_childs / ((lf_div_t*)widget)->_column_count);
  uint32_t n_columns = ((lf_div_t*)widget)->_column_count;

  float row_heights[n_rows];
  float column_widths[n_columns];
  memset(row_heights, 0, sizeof(row_heights));
  memset(column_widths, 0, sizeof(column_widths));

  row_i = 0;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    if(i % n_columns == 0 && i != 0) {
      row_i++;
    }
    
    lf_widget_t* child = widget->childs[i];
    float widget_h = effective_widget_size(child).y;
    if(widget_h > row_heights[row_i] && !child->changed_by_layout) {
      row_heights[row_i] = widget_h; 
    }
    
    float widget_w = effective_widget_size(child).x + child->props.padding_left + child->props.padding_right;
    uint32_t col_i = i % n_columns;
    if(widget_w > column_widths[col_i]) {
      column_widths[col_i] = widget_w;
    }
  }

  float total_child_h = 0.0f, total_child_w = 0.0f;
  for(uint32_t i = 0; i < n_rows; i++) {
    total_child_h += row_heights[i];
  }
  for(uint32_t i = 0; i < n_columns; i++) {
    total_child_w += column_widths[i];
  }
  vec2s offset = (vec2s){
    .x = (widget->parent->container.size.x - total_child_w) / 2.0f,
    .y = (widget->parent->container.size.y - total_child_h) / 2.0f,
  };

  bool is_div = widget->type == WidgetTypeDiv;
  bool centered_horz = is_div && lf_alignment_flag_exists(&((lf_div_t*)widget)->flags, AlignCenterHorizontal);
  bool centered_vert = is_div && lf_alignment_flag_exists(&((lf_div_t*)widget)->flags, AlignCenterVertical);

  if(!is_div || !centered_horz)
    offset.x = 0;
  if(!is_div || !centered_vert) 
    offset.y = 0;

  float x_before = widget->container.pos.x + offset.x;
  float y_before = widget->container.pos.x + offset.y;

  float x_ptr = x_before + widget->props.padding_left;
  float y_ptr = y_before + widget->props.padding_top;

  row_i = 0;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if(i % n_columns == 0 && i != 0) {
      y_ptr += row_heights[row_i++];
      x_ptr = x_before + widget->props.padding_left;
    }

    uint32_t col_i = i % n_columns;
    float w = column_widths[col_i] - child->props.margin_left - child->props.margin_right;
    float ph = (w - child->container.size.x) / 2.0f;
    float h = row_heights[row_i] - child->props.margin_top - child->props.margin_bottom;
    float pv = (h - child->container.size.y) / 2.0f;
    
    child->props.padding_left = ph; 
    child->props.padding_right = ph;
    child->props.padding_top = pv; 
    child->props.padding_bottom = pv;

    child->container.pos.x = x_ptr + child->props.margin_left;
    child->container.pos.y = y_ptr + child->props.margin_top;
    
    x_ptr += column_widths[col_i]; 
  }
  if(!fixed_w) {
    widget->container.size.x = size_of_childs(widget).x;
  }
  if(!fixed_h) {
    widget->container.size.y = size_of_childs(widget).y;
  }
}

void lf_alignment_flag_set(
    uint32_t* flags, 
    lf_alignment_flag_t flag) {
  *flags |= flag;
}

void lf_alignment_flag_unset(
    uint32_t* flags,
    lf_alignment_flag_t flag) {
  *flags &= ~flag;
}

bool lf_alignment_flag_exists(
    uint32_t* flags,
    lf_alignment_flag_t flag) {
  return (*flags & flag) != 0;
}
