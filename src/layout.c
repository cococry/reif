#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/widgets/div.h"
#include "../include/leif/util.h"

static void adjust_widget_size(lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h);
static vec2s effective_widget_size(lf_widget_t* widget);

void 
adjust_widget_size(lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h) {
  bool fixed_w = false, fixed_h = false;
  if(widget->_fixed_width) { 
    fixed_w = true;
  }
  if(widget->_fixed_height)
    fixed_h = true;

  if(widget->parent && !fixed_w) {
    widget->container.size.x = widget->parent->container.size.x - 
    (widget->props.padding_left + widget->props.padding_right) - 
    (widget->props.margin_left + widget->props.margin_right); 
    
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
  bool fixed_w, fixed_h;
  adjust_widget_size(widget, &fixed_w, &fixed_h);
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props = widget->childs[i]->_initial_props;
  }

  vec2s offset = (vec2s){
    .x = 0,
    .y = 0 
  };

  float height = 0.0f, width = 0.0f;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    vec2s effictive = LF_WIDGET_SIZE_V2(child);
    height += effictive.y + child->props.margin_top + child->props.margin_bottom;
    float w = effictive.x + child->props.margin_left + child->props.margin_right;
    if(w > width)
      width = w;
  }

  if(lf_alignment_flag_exists(&widget->alignment_flags, AlignCenterVertical)) {
    offset.y = (widget->container.size.y - height) / 2.0f;
  }

  float y_before = widget->container.pos.y + offset.y;

  float y_ptr = y_before + widget->props.padding_top;

  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    vec2s effictive = LF_WIDGET_SIZE_V2(child);

    if(lf_alignment_flag_exists(&widget->alignment_flags, AlignCenterHorizontal))
      offset.x = (widget->container.size.x - (effictive.x + child->props.margin_left + child->props.margin_right)) / 2.0f;

    child->container.pos.x = widget->container.pos.x
      + widget->props.padding_left + child->props.margin_left + offset.x;

    child->container.pos.y = y_ptr + child->props.margin_top; 


    y_ptr += effictive.y + child->props.margin_bottom + child->props.margin_top; 
  }

  if(!fixed_h)
    widget->container.size.y = height; 
  if(!fixed_w)
    widget->container.size.x = width; 
}

void 
lf_layout_horizontal(lf_widget_t* widget) {
  (void)widget;
}

void lf_layout_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if (widget->type != WidgetTypeDiv) return;
  if (!widget->num_childs) return;
  if (!widget->parent) return;

  for (uint32_t i = 0; i < widget->num_childs; i++) {
    widget->childs[i]->props = widget->childs[i]->_initial_props;
  }

  lf_widget_props_t p = widget->props;

  bool fixed_w, fixed_h;
  adjust_widget_size(widget, &fixed_w, &fixed_h);

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
    if (!child->changed_by_layout || row_heights[row_i] == 0) {
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

  bool centered_horz = lf_alignment_flag_exists(&widget->alignment_flags, AlignCenterHorizontal);
  bool centered_vert = lf_alignment_flag_exists(&widget->alignment_flags, AlignCenterVertical);

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
