#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/widgets/div.h"
#include "../include/leif/util.h"
#include <unistd.h>

static void adjust_widget_size(lf_ui_state_t* ui, lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h, bool horizontal);
static vec2s effective_widget_size(lf_widget_t* widget);
/*static vec2s widget_avail_size(lf_widget_t* widget, lf_widget_t* ignore);*/

void adjust_widget_size(lf_ui_state_t* ui, lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h, bool horizontal) {
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
  if (resizable && (widget->sizing_type == SizingFitToParent || (widget->sizing_type == SizingGrow))) {
    widget->container.size.x = parent_inner_width - 
      widget->props.margin_left - widget->props.margin_right -
      widget->props.padding_left - widget->props.padding_right;
  } 
  if (widget->_fixed_width && widget->_width_percent != 0.0f) {
    widget->container.size.x = parent_inner_width * widget->_width_percent - (widget->props.padding_left + widget->props.padding_right);
  }

  // Height sizing
  if (resizable && (widget->sizing_type == SizingFitToParent || (widget->sizing_type == SizingGrow))) {
    widget->container.size.y = parent_inner_height - (widget->props.margin_top + widget->props.margin_bottom);
  }
  if (widget->_fixed_height && widget->_height_percent != 0.0f) {
    widget->container.size.y = parent_inner_height * widget->_height_percent; 
  }

  // Set output fixed flag values
  if (o_fixed_w) {
    *o_fixed_w = (horizontal) ? widget->_fixed_width || widget->sizing_type != SizingFitToContent : widget->_fixed_width;
  }
  if (o_fixed_h) {
    *o_fixed_h = (horizontal) ? widget->_fixed_height : widget->_fixed_height || widget->sizing_type != SizingFitToContent;
  }
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

/*vec2s 
widget_avail_size(lf_widget_t* widget, lf_widget_t* ignore) {
  vec2s avail = widget->container.size;

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    if(widget->childs[i] == ignore) continue;
    vec2s effective = effective_widget_size(widget->childs[i]);
    avail.x -= effective.x; 
    avail.y -= effective.y; 
  }
  return avail;
}*/

vec2s measure_children(lf_widget_t* widget, vec2s* o_max) {
  vec2s size = (vec2s){
    .x = 0,
    .y = 0
  };
  vec2s max = (vec2s){
    .x = 0,
    .y = 0
  };

  vec2s ptr = (vec2s){
    .x = widget->container.pos.x,
    .y = widget->container.pos.y 
  };
  vec2s ptr_before = ptr;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;

    vec2s size = LF_WIDGET_SIZE_V2(child); 
    ptr.y += child->props.margin_top;
    child->container.pos.x = widget->container.pos.x + child->props.margin_left; 
    child->container.pos.y = ptr.y;
    ptr.y += size.y + child->props.margin_bottom;

    if(size.x + child->props.margin_left + child->props.margin_right > max.x) {
      max.x = size.x + child->props.margin_left + child->props.margin_right;
    }
    if(size.y + child->props.margin_top + child->props.margin_bottom > max.y) {
      max.y = size.y + child->props.margin_top + child->props.margin_bottom;
    }
  }

  size.x = ptr.x - ptr_before.x; 
  size.y = ptr.y - ptr_before.y;

  if(o_max) {
    *o_max = max;
  }

  return size;
}

static void reset_widget_props(lf_widget_t* widget) {
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props = widget->childs[i]->_initial_props;
  }
} 

void 
lf_layout_vertical(lf_ui_state_t* ui, lf_widget_t* widget) {
  reset_widget_props(widget);
  lf_widget_apply_size_hints(widget);

  if(widget->sizing_type == SizingFitToContent) {
    vec2s max;
    widget->container.size.y = measure_children(widget, &max).y;
    widget->container.size.x = max.x;
  } else if(widget->sizing_type == SizingFitToParent && widget->parent){
    widget->container.size = widget->parent->container.size;
  }

  vec2s ptr = (vec2s){
    .x = widget->container.pos.x,
    .y = widget->container.pos.y 
  };
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;

    vec2s size = LF_WIDGET_SIZE_V2(child); 
    ptr.y += child->props.margin_top;
    child->container.pos.x = widget->container.pos.x + child->props.margin_left; 
    child->container.pos.y = ptr.y;
    ptr.y += size.y + child->props.margin_bottom; 
  }
  lf_widget_apply_size_hints(widget);
}

void lf_layout_horizontal(lf_ui_state_t* ui, lf_widget_t* widget) {
  /*bool fixed_w = false, fixed_h = false;

  lf_widget_apply_size_hints(widget);
  adjust_widget_size(ui, widget, &fixed_w, &fixed_h, true);

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props = widget->childs[i]->_initial_props;
  }

  vec2s offset = { 
    .x = widget->props.padding_left, 
    .y = widget->props.padding_top 
  };

  float total_width = 0.0f, max_height = 0.0f;

  bool centered_horz = lf_flag_exists(&widget->alignment_flags, AlignCenterHorizontal) && 
    widget->justify_type != JustifyEnd;

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    if (!child->visible) continue;

    vec2s effective_size = LF_WIDGET_SIZE_V2(child);

    total_width += effective_size.x;
    total_width += child->props.margin_left;
    total_width += child->props.margin_right;

    float child_height = effective_size.y + child->props.margin_top + child->props.margin_bottom;
    if (child_height > max_height) {
      max_height = child_height;
    }
  }

  float s = (widget->container.size.x - total_width) / (widget->num_childs - 1);

  if (centered_horz) {
    offset.x = (lf_widget_width(widget) - total_width) / 2.0f;
  }

  float x_ptr = widget->container.pos.x + offset.x;
  if (widget->justify_type == JustifyEnd) {
    x_ptr = widget->container.pos.x + lf_widget_width(widget) - widget->props.padding_right;
  }

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

    if (!child->visible) continue;

    vec2s effective_size = LF_WIDGET_SIZE_V2(child);

    bool centered_vertical = lf_flag_exists(&widget->alignment_flags, AlignCenterVertical);
    if (centered_vertical) {
      offset.y = (lf_widget_height(widget) - effective_size.y) / 2.0f;
    }

    child->container.pos.y = widget->container.pos.y + offset.y + ((!centered_vertical) ? child->props.margin_top : 0.0f);

    if (widget->justify_type == JustifyEnd) {
      x_ptr -= effective_size.x + child->props.margin_right;
      child->container.pos.x = x_ptr;
      x_ptr -= child->props.margin_left;
    } else {
      x_ptr += child->props.margin_left;
      child->container.pos.x = x_ptr;

      x_ptr += effective_size.x + child->props.margin_right + (widget->justify_type == JustifySpaceBetween ? s : 0.0f);
    }
  }

  if (!fixed_w) {
    widget->container.size.x = total_width;
  }

  if (!fixed_h) {
    widget->container.size.y = max_height;
  }

  if(widget->sizing_type == SizingGrow) {
    float h =  widget_avail_size(widget->parent, widget).y - 
    (widget->props.padding_top + widget->props.padding_bottom * 2);

    widget->container.size.y = h;

  }
  lf_widget_apply_size_hints(widget);*/
}


void lf_layout_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget) return;
  if (widget->type != WidgetTypeDiv) return;
  if (!widget->num_childs) return;
  if (!widget->parent) return;

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props = widget->childs[i]->_initial_props;
  }

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
    float widget_h = effective_widget_size(child).y + child->props.margin_top + child->props.margin_bottom;

    // Ensure row height is updated at least once, even if `changed_by_layout` is true.
    if (!child->_changed_by_layout || row_heights[row_i] == 0) {
      row_heights[row_i] = fmax(row_heights[row_i], widget_h);
    }

    float widget_w = effective_widget_size(child).x + child->props.margin_left + child->props.margin_right;
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

  bool centered_horz = lf_flag_exists(&widget->alignment_flags, AlignCenterHorizontal);
  bool centered_vert = lf_flag_exists(&widget->alignment_flags, AlignCenterVertical);

  if (!centered_horz) offset.x = 0;
  if (!centered_vert) offset.y = 0;

  float x_start = widget->container.pos.x + offset.x;
  float y_start = widget->container.pos.y + offset.y;

  float x_ptr = x_start + p.padding_left;
  float y_ptr = y_start + p.padding_top;

  row_i = 0;
  for (uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];

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

    child->container.pos.x = x_ptr + child->props.margin_left;
    child->container.pos.y = y_ptr + child->props.margin_top;

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

