#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/widgets/div.h"
#include "../include/leif/util.h"
#include <unistd.h>

static void adjust_widget_size(lf_ui_state_t* ui, lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h, bool horizontal);

void adjust_widget_size(lf_ui_state_t* ui, lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h, bool horizontal) {
  if (!widget->visible) return;
  bool resizable = (!widget->_fixed_width && horizontal) || (!widget->_fixed_height && !horizontal); 
  if (!widget || !widget->parent) {
    if (o_fixed_w) *o_fixed_w = widget ? widget->_fixed_width : false;
    if (o_fixed_h) *o_fixed_h = widget ? widget->_fixed_height : false;
    return;
  }

  lf_widget_t* parent = widget->parent;

  float parent_inner_width = parent->container.size.x - (parent->props.padding_left - parent->props.padding_right);
  float parent_inner_height = parent->container.size.y - (parent->props.padding_bottom);


  // Width sizing
  bool adjustable = (widget->sizing_type == LF_SIZING_FIT_CONTENT || (widget->sizing_type == LF_SIZING_GROW));
  if (resizable && adjustable) {
    widget->container.size.x = parent_inner_width - 
      widget->props.margin_left - widget->props.margin_right -
      widget->props.padding_left - widget->props.padding_right;
  } 

  // Height sizing
  if (resizable && adjustable) {
    widget->container.size.y = parent_inner_height - (widget->props.margin_top + widget->props.margin_bottom);
  }

  // Set output fixed flag values
  if (o_fixed_w) {
    *o_fixed_w = (horizontal) ? widget->_fixed_width || widget->sizing_type != LF_SIZING_FIT_CONTENT : widget->_fixed_width;
  }
  if (o_fixed_h) {
    *o_fixed_h = (horizontal) ? widget->_fixed_height : widget->_fixed_height || widget->sizing_type != LF_SIZING_FIT_CONTENT;
  }
}


static void widget_grow_horz(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget->visible) return;
  if(!widget->_fixed_height) {
    widget->container.size.y = widget->parent->container.size.y - 
      widget->props.padding_top  - widget->props.padding_bottom - 
      widget->props.margin_top   - widget->props.margin_bottom;
  }
  float avail = widget->parent->container.size.x;
  for(uint32_t i = 0; i < widget->parent->num_childs; i++) {
    if(widget->parent->childs[i] == widget) continue;
    avail -= lf_widget_effective_size(widget->parent->childs[i]).x;
  } 
  if(!widget->_fixed_width) {
    widget->container.size.x = avail - widget->props.margin_left - widget->props.margin_right
      - widget->props.padding_left - widget->props.padding_right; 
  }
}

static void widget_grow_vert(lf_ui_state_t* ui, lf_widget_t* widget) {

  if (!widget->visible) return;
  if(!widget->_fixed_width) {
    widget->container.size.x = widget->parent->container.size.x 
      - widget->props.padding_left - widget->props.padding_right - 
      widget->props.margin_left  - widget->props.margin_right;
  }
  float avail = widget->parent->container.size.y;
  for(uint32_t i = 0; i < widget->parent->num_childs; i++) {
    if(widget->parent->childs[i] == widget) continue;
    avail -= lf_widget_effective_size(widget->parent->childs[i]).y;
  } 
  if(!widget->_fixed_height) {
    widget->container.size.y = avail - widget->props.margin_top - widget->props.margin_bottom
      - widget->props.padding_top - widget->props.padding_bottom;
  }
}

void 
lf_layout_vertical(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget->visible) return;
  lf_widget_props_t widget_props = widget->props; 

  vec2s child_size = lf_widget_measure_children(widget, NULL);
   vec2s offset = (vec2s){
    .x = widget_props.padding_left,
    .y = widget_props.padding_top
  };

  bool centered_horizontal = lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_HORIZONTAL); 

  if(lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_VERTICAL) && widget->sizing_type != LF_SIZING_FIT_CONTENT) {
    offset.y = (lf_widget_height_ex(widget, widget_props) - child_size.y) / 2.0f;
  }

  if(centered_horizontal) {
    offset.x = 0;
  }

  vec2s ptr = (vec2s) {
    .x = widget->container.pos.x + offset.x, 
    .y = widget->container.pos.y + offset.y 
  };
  if(widget->justify_type == LF_JUSTIFY_END) {
    ptr.y = widget->container.pos.y + widget->container.size.y;
  }
  float s = (widget->container.size.y - child_size.y) / (widget->num_childs - 1);

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;

    vec2s size = LF_WIDGET_SIZE_V2(child);
    if(widget->justify_type == LF_JUSTIFY_START)
      ptr.y += child->props.margin_top;
    else if(widget->justify_type == LF_JUSTIFY_END) 
      ptr.y -= size.y + child->props.margin_bottom;

    float centering_offset = (lf_widget_width_ex(widget, widget->props) - LF_WIDGET_SIZE_V2(child).x) / 2.0f;

    lf_widget_set_pos_x(child, ptr.x + ((centered_horizontal) ? centering_offset : child->props.margin_left)); 
    lf_widget_set_pos_y(child, ptr.y);

    if(widget->justify_type == LF_JUSTIFY_START)
      ptr.y += size.y + child->props.margin_bottom; 
    else if (widget->justify_type == LF_JUSTIFY_SPACE_BETWEEN) 
      ptr.y += size.y + child->props.margin_bottom + (widget->justify_type == LF_JUSTIFY_SPACE_BETWEEN  ?  s : 0.0f); 
    else
      ptr.y -= child->props.margin_top;
  }
  lf_widget_apply_size_hints(widget);
}



void lf_layout_horizontal(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget->visible) return;
  lf_widget_props_t widget_props = widget->props; 

  vec2s child_size = lf_widget_measure_children(widget, NULL);
  vec2s offset = (vec2s){
    .x = widget_props.padding_left,
    .y = widget_props.padding_top
  };
  if(lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_HORIZONTAL)) {
    offset.x = (lf_widget_width_ex(widget, widget_props) - child_size.x) / 2.0f;
  }
  if(lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_VERTICAL)) {
    offset.y = 0;
  }
  vec2s ptr = (vec2s){
    .x = widget->container.pos.x + offset.x,
    .y = widget->container.pos.y + offset.y 
  };

  if(widget->justify_type == LF_JUSTIFY_END) {
    ptr.x = widget->container.pos.x + widget->container.size.x;
  }

  float s = (MAX(widget->total_child_size.x, widget->container.size.x) - child_size.x) / (widget->num_childs - 1);

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;

    vec2s size = LF_WIDGET_SIZE_V2(child);

    bool centered_vertical = lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_VERTICAL);
    if(widget->justify_type == LF_JUSTIFY_START)
      ptr.x += child->props.margin_left;
    else if(widget->justify_type == LF_JUSTIFY_END) 
      ptr.x -= size.x + child->props.margin_right;
   
    lf_widget_set_pos_y(child, ptr.y + ((centered_vertical) ? (
      LF_WIDGET_SIZE_V2(widget).y - 
      LF_WIDGET_SIZE_V2(child).y) / 2.0f :
      child->props.margin_top));

    lf_widget_set_pos_x(child, ptr.x);

    if(widget->justify_type == LF_JUSTIFY_START)
      ptr.x += size.x + child->props.margin_right; 
    else if (widget->justify_type == LF_JUSTIFY_SPACE_BETWEEN) 
      ptr.x += size.x + child->props.margin_right + (widget->justify_type == LF_JUSTIFY_SPACE_BETWEEN ?  s : 0.0f); 
    else
      ptr.x -= child->props.margin_left;
  }

  lf_widget_apply_size_hints(widget);
}


void lf_layout_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget) return;
  if (widget->type != LF_WIDGET_TYPE_DIV) return;
  if (!widget->num_childs) return;
  if (!widget->parent) return;
  if (!widget->visible) return;

  lf_widget_props_t p = widget->props;

  bool fixed_w = false, fixed_h = false;
  adjust_widget_size(ui, widget, &fixed_w, &fixed_h, true);

  uint32_t n_columns = ((lf_div_t*)widget)->_column_count;
  uint32_t n_rows = (widget->num_childs + n_columns - 1) / n_columns;

  float* row_heights = (float*)calloc(n_rows, sizeof(float));
  float* column_widths = (float*)calloc(n_columns, sizeof(float));

  uint32_t row_i = 0;
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    if (i % n_columns == 0 && i != 0) {
      row_i++;
    }

    lf_widget_t* child = widget->childs[i];
    float widget_h = lf_widget_effective_size(child).y + child->props.margin_top + child->props.margin_bottom;

    if (!child->_changed_by_layout || row_heights[row_i] == 0) {
      row_heights[row_i] = fmax(row_heights[row_i], widget_h);
    }

    float widget_w = lf_widget_effective_size(child).x + child->props.margin_left + child->props.margin_right;
    uint32_t col_i = i % n_columns;
    column_widths[col_i] = fmax(column_widths[col_i], widget_w);
  }

  float total_child_h = 0.0f, total_child_w = 0.0f;
  for (uint32_t i = 0; i < n_rows; i++) {
    total_child_h += row_heights[i];
  }
  for (uint32_t i = 0; i < n_columns; i++) {
    total_child_w += column_widths[i];
  }

  vec2s offset = (vec2s){
    .x = (widget->container.size.x - total_child_w) / 2.0f,
    .y = (widget->container.size.y - total_child_h) / 2.0f,
  };

  bool centered_horz = lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_HORIZONTAL);
  bool centered_vert = lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_VERTICAL);

  if (!centered_horz) offset.x = 0;
  if (!centered_vert) offset.y = 0;

  float x_start = widget->container.pos.x + offset.x;
  float y_start = widget->container.pos.y + offset.y;

  float x_ptr = x_start + p.padding_left;
  float y_ptr = y_start + p.padding_top;

  row_i = 0;
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
  if (!child->visible) return;

    if (i % n_columns == 0 && i != 0) {
      y_ptr += row_heights[row_i++];
      x_ptr = x_start + p.padding_left;
    }

    uint32_t col_i = i % n_columns;
    float w = column_widths[col_i] - child->props.margin_left - child->props.margin_right;
    float h = row_heights[row_i] - child->props.margin_top - child->props.margin_bottom;

    child->props.padding_left = (w - child->container.size.x) / 2.0f;
    child->props.padding_right = child->props.padding_left;
    child->props.padding_top = (h - child->container.size.y) / 2.0f;
    child->props.padding_bottom = child->props.padding_top;

    lf_widget_set_pos_x(child,  x_ptr + child->props.margin_left);
    lf_widget_set_pos_y(child, y_ptr + child->props.margin_top);

    x_ptr += column_widths[col_i];
  }

  if (!fixed_w) {
    widget->container.size.x = total_child_w;
  }
  if (!fixed_h) {
    widget->container.size.y = total_child_h;
  }

  free(row_heights);
  free(column_widths);
}


void lf_size_calc_vertical(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget->_needs_size_calc) return;
  lf_widget_props_t props = widget->props;
  float min_width = -1.0f;
  if(widget->sizing_type == LF_SIZING_FIT_PARENT) {
    if (widget->parent && !widget->_fixed_width) {
      min_width = widget->parent->container.size.x
        - props.padding_left - props.padding_right
        - props.margin_left  - props.margin_right;
      widget->container.size.x = min_width;
    }
  }

  lf_widget_apply_size_hints(widget);

  for (size_t i = 0; i < widget->num_childs; i++) {
    if(widget->childs[i]->size_calc)
      widget->childs[i]->size_calc(ui, widget->childs[i]);  
  }
  vec2s child_max;
  vec2s child_size = lf_widget_measure_children(widget, &child_max);

  if(widget->sizing_type == LF_SIZING_GROW && widget->parent) {
    if (widget->parent->layout_type == LF_LAYOUT_HORIZONTAL) {
      widget_grow_horz(ui, widget);
    } else if (widget->parent->layout_type == LF_LAYOUT_VERTICAL) {
      widget_grow_vert(ui, widget);
    }
  } else if(widget->sizing_type == LF_SIZING_FIT_CONTENT) {
    if (!widget->_fixed_width) {
      widget->container.size.x = child_max.x;
    }
  }

  if (widget->sizing_type != LF_SIZING_GROW &&
      !widget->_fixed_height) {
    widget->container.size.y = child_size.y;
  }

  lf_widget_apply_size_hints(widget);

  widget->_needs_size_calc = false;
  widget->total_child_size.y = child_size.y;
  widget->total_child_size.x = child_max.x;

  if(fabs(widget->scroll_offset.y) > 
    fabs(widget->container.size.y - widget->total_child_size.y)) {
    widget->scroll_offset.y = 0; 
  }
  if(fabs(widget->scroll_offset.x) > 
    fabs(widget->container.size.x - widget->total_child_size.x)) {
    widget->scroll_offset.x = 0; 
  }
  widget->_changed_size = false;
}


void 
lf_size_calc_horizontal(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget->_needs_size_calc) return;
  lf_widget_props_t props = widget->_rendered_props;
  float min_width = -1.0f;
  if(widget->sizing_type == LF_SIZING_FIT_PARENT) {
    if (widget->parent && !widget->_fixed_width) {
      min_width = widget->parent->container.size.x 
        - props.padding_left - props.padding_right 
        - props.margin_left  - props.margin_right;
      widget->container.size.x = min_width;
    }
  }
  lf_widget_apply_size_hints(widget);

  for (size_t i = 0; i < widget->num_childs; i++) {
    if(widget->childs[i]->size_calc)
      widget->childs[i]->size_calc(ui, widget->childs[i]);  
  }

  vec2s child_max; 
  vec2s child_size = lf_widget_measure_children(widget, &child_max);

  if(widget->sizing_type == LF_SIZING_FIT_CONTENT) {
    if (!widget->_fixed_width) {
      widget->container.size.x = child_size.x;
    }
  } else if(widget->sizing_type == LF_SIZING_GROW && widget->parent) {
    if (widget->parent->layout_type == LF_LAYOUT_HORIZONTAL) {
      widget_grow_horz(ui, widget);
    } else if (widget->parent->layout_type == LF_LAYOUT_VERTICAL) {
      widget_grow_vert(ui, widget);
    }
  }

  if (!widget->_fixed_height && widget->sizing_type != LF_SIZING_GROW) {
    widget->container.size.y = child_max.y;
  }

  lf_widget_apply_size_hints(widget);
  widget->_needs_size_calc = false;
  widget->total_child_size.x = child_size.x;
  widget->total_child_size.y = child_max.y;
  if(fabs(widget->scroll_offset.y) > 
    fabs(widget->container.size.y - widget->total_child_size.y)) {
    widget->scroll_offset.y = 0; 
  }
  if(fabs(widget->scroll_offset.x) > 
    fabs(widget->container.size.x - widget->total_child_size.x)) {
    widget->scroll_offset.x = 0; 
  }
  widget->_changed_size = false;
}

void 
lf_size_calc_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  (void)widget;
}
