#include "../include/leif/layout.h"
#include "../include/leif/widget.h"
#include "../include/leif/widgets/div.h"
#include "../include/leif/widgets/text.h"
#include "../include/leif/util.h"
#include <unistd.h>

static void adjust_widget_size(lf_ui_state_t* ui, lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h, bool horizontal);

void adjust_widget_size(lf_ui_state_t* ui, lf_widget_t* widget, bool* o_fixed_w, bool* o_fixed_h, bool horizontal) {
  if (!widget->visible) return;
  if (!widget || !widget->parent) {
    if (o_fixed_w) *o_fixed_w = widget ? widget->_fixed_width : false;
    if (o_fixed_h) *o_fixed_h = widget ? widget->_fixed_height : false;
    return;
  }

  if (o_fixed_w) {
    *o_fixed_w = (horizontal) ? widget->_fixed_width || widget->sizing_type != LF_SIZING_FIT_CONTENT : widget->_fixed_width;
  }
  if (o_fixed_h) {
    *o_fixed_h = (horizontal) ? widget->_fixed_height : widget->_fixed_height || widget->sizing_type != LF_SIZING_FIT_CONTENT;
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

  vec2s offset = (vec2s){
    .x = widget_props.padding_left,
    .y = widget_props.padding_top
  };

  bool centered_horizontal = lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_HORIZONTAL); 

  if(lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_VERTICAL) && widget->sizing_type != LF_SIZING_FIT_CONTENT) {
    offset.y = (lf_widget_height_ex(widget, widget_props) - widget->total_child_size.y) / 2.0f;
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
  float s = (widget->container.size.y - widget->total_child_size.y) / (widget->num_childs - 1);

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

  vec2s offset = (vec2s){
    .x = widget_props.padding_left,
    .y = widget_props.padding_top
  };
  if(lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_HORIZONTAL)) {
    offset.x = (lf_widget_width_ex(widget, widget_props) - widget->total_child_size.x) / 2.0f;
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

  float s = (MAX(widget->total_child_size.x, widget->container.size.x) - widget->total_child_size.x) / (widget->num_childs - 1);

  float start_x = ptr.x;
  float line_h = 0.0f;

  uint32_t widgets_on_this_line = 0;
  bool nowrapping = false;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if(child->_abs_x_percent != -1.0f && child->_positioned_absolute_x) {
      child->container.pos.x = widget->container.pos.x + widget->props.padding_left + 
        (widget->container.size.x - lf_widget_width(child)) * (child->_abs_x_percent / 100.0f); 
    }
    if (!child->visible) continue;
    if(child->sizing_type == LF_SIZING_GROW) nowrapping = true;

    vec2s size = LF_WIDGET_SIZE_V2(child);


    bool centered_vertical = lf_flag_exists(&widget->alignment_flags, LF_ALIGN_CENTER_VERTICAL);
    if(widget->justify_type == LF_JUSTIFY_START)
      ptr.x += child->props.margin_left;
    else if(widget->justify_type == LF_JUSTIFY_END) 
      ptr.x -= size.x + child->props.margin_right;

    lf_widget_set_pos_y(child, ptr.y + ((centered_vertical) ? (
                        (LF_WIDGET_SIZE_V2(widget).y - 
                        (child->type != LF_WIDGET_TYPE_SLIDER ? 
                        LF_WIDGET_SIZE_V2(child).y : child->container.size.y) ) / 2.0f) :
                        child->props.margin_top));

    lf_widget_set_pos_x(child, ptr.x);

    if(widget->justify_type == LF_JUSTIFY_START)
      ptr.x += size.x + child->props.margin_right; 
    else if (widget->justify_type == LF_JUSTIFY_SPACE_BETWEEN) 
      ptr.x += size.x + child->props.margin_right + (widget->justify_type == LF_JUSTIFY_SPACE_BETWEEN ?  s : 0.0f); 
    else
      ptr.x -= child->props.margin_left;
    if(child->_abs_y_percent != -1.0f && child->_positioned_absolute_y) {
      child->container.pos.x = widget->container.pos.x + widget->props.padding_left; 
      child->container.pos.y = widget->container.pos.y + widget->props.padding_top + 
        (widget->container.size.y - lf_widget_height(child)) * (child->_abs_y_percent / 100.0f); 
    }
    if(!nowrapping) {
      if (widgets_on_this_line > 0 &&
        ptr.x - widget->props.padding_right > widget->container.pos.x + widget->container.size.x) {
        lf_widget_set_pos_x(child, start_x + child->props.margin_left);
        lf_widget_set_pos_y(child, child->container.pos.y + line_h); 
        ptr.x = start_x + size.x + child->props.margin_left + child->props.margin_right; 
        ptr.y += line_h;
        line_h = size.y;
        widgets_on_this_line = 0;
      }
    }
    line_h  = MAX(lf_widget_effective_size(child).y, line_h);
    widgets_on_this_line++;

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

void 
mark_widget_and_children_finished(lf_widget_t* widget) {
  if (widget == NULL) return;
  widget->_changed_size = false;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    mark_widget_and_children_finished(widget->childs[i]);
  }
}

void lf_size_calc_vertical(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget->_needs_size_calc) return;

  // first pass all non-growing children
  float total_nongrow_height = 0.0f;
  int grow_count = 0;

  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;
    if (child->sizing_type == LF_SIZING_GROW) {
      grow_count++;
    } else {
      if (child->size_calc) child->size_calc(ui, child);
      total_nongrow_height += lf_widget_effective_size(child).y;
    }
  }

  // second pass all growing children 
  float available = widget->container.size.y - total_nongrow_height;
  float shared_height = (grow_count > 0) ? (available / (float)grow_count) : 0.0f;

  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;
    if (child->sizing_type == LF_SIZING_GROW && !child->_fixed_height) {
      child->container.size.y =
        MAX(0, shared_height 
            - child->props.margin_top - child->props.margin_bottom
            - child->props.padding_top - child->props.padding_bottom);
      if (!child->_fixed_width) {
        child->container.size.x = widget->container.size.x
          - child->props.margin_left - child->props.margin_right
          - child->props.padding_left - child->props.padding_right;
      }
      if (child->size_calc && child->_needs_size_calc) child->size_calc(ui, child); 
    }
  }

  vec2s child_max;
  vec2s child_size = lf_widget_measure_children(widget, &child_max);
  if(widget->sizing_type == LF_SIZING_FIT_CONTENT) {
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
  mark_widget_and_children_finished(widget);
}

void text_unbound_pass(lf_ui_state_t* ui, lf_text_t* text) {
  if(!text->base._needs_size_calc) return;
  lf_widget_t* widget = &text->base;
  if(widget->parent->sizing_type == LF_SIZING_FIT_CONTENT && !widget->_changed_size) {
    return;
  }
  vec2s text_pos = (vec2s){
    .x = widget->container.pos.x + widget->props.padding_left, 
    .y =  widget->container.pos.y + widget->props.padding_top 
  };
  lf_text_dimension_t text_dimension = ui->render_get_paragraph_dimension(
    ui->render_state,
    text->label,
    text_pos,
    text->font.font,
    (lf_paragraph_props_t){
      .wrap = -1.0f, // unbound wrap 
      .align = widget->props.text_align
    }
  );
  if((text->_text_dimension.width != text_dimension.width || 
      text->_text_dimension.height != text_dimension.height)) {
    widget->_changed_size = true;
  }
  if(!widget->_fixed_width)
    text->base.container.size.x = text_dimension.width; 
  if(!widget->_fixed_height)
    text->base.container.size.y = text_dimension.height;

  text->_text_dimension = text_dimension;
}

void grow_pass(lf_ui_state_t* ui, lf_widget_t* widget) {
}
void lf_size_calc_horizontal(lf_ui_state_t* ui, lf_widget_t* widget) {
  if (!widget->_needs_size_calc) return;

  // first pass: calculate unbound sizes for text widgets
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if(child->type == LF_WIDGET_TYPE_TEXT) {
      child->_needs_size_calc = true;
      text_unbound_pass(ui, (lf_text_t*)child);
    }
  }
  // then pass all non-growing non-text children
  float total_nongrow_width = 0.0f;
  int grow_count = 0;

  // second pass, calculate growing children 
  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;
    if (child->sizing_type == LF_SIZING_GROW) {
      grow_count++;
    } else {
      total_nongrow_width += lf_widget_effective_size(child).x;
    }
  }
  float available = widget->container.size.x - total_nongrow_width;
  float shared_width = (grow_count > 0) ? (available / (float)grow_count) : 0.0f;

  vec2s child_max;
  vec2s child_size = lf_widget_measure_children_wrapped(ui, widget, &child_max);
  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;
    if (child->sizing_type == LF_SIZING_GROW && !child->_fixed_width) {
      child->container.size.x =
        MAX(-(child->props.padding_left + child->props.padding_right), shared_width - widget->props.padding_right  - 
            child->props.padding_right );
      if (!child->_fixed_height) {
        child->container.size.y = child_max.y;
      }
    }
  }

  // third pass, calculate size of wrapped texts 
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if(child->type == LF_WIDGET_TYPE_TEXT && child->size_calc) {
      child->_needs_size_calc = true;
      child->size_calc(ui, child); 
    }
  }
  // then pass all non-growing non-text children
  total_nongrow_width = 0.0f;
  grow_count = 0;

  // second pass, calculate growing children 
  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;
    if (child->sizing_type == LF_SIZING_GROW) {
      grow_count++;
    } else {
      total_nongrow_width += lf_widget_effective_size(child).x;
    }
  }
  available = widget->container.size.x - total_nongrow_width;
  shared_width = (grow_count > 0) ? (available / (float)grow_count) : 0.0f;

  child_size = lf_widget_measure_children_wrapped(ui, widget, &child_max);
  for (size_t i = 0; i < widget->num_childs; i++) {
    lf_widget_t* child = widget->childs[i];
    if (!child->visible) continue;
    if (child->sizing_type == LF_SIZING_GROW && !child->_fixed_width) {
      child->container.size.x =
        MAX(-(child->props.padding_left + child->props.padding_right), shared_width - widget->props.padding_right  - 
            child->props.padding_right);
      if (!child->_fixed_height) {
        child->container.size.y = child_max.y;
      }
    }
  }

  if (widget->sizing_type == LF_SIZING_FIT_CONTENT && !widget->_fixed_width) {
    widget->container.size.x = child_size.x;
  }

  if (!widget->_fixed_height && widget->sizing_type != LF_SIZING_GROW) {
    widget->container.size.y = child_max.y;
  }

  lf_widget_apply_size_hints(widget);
  widget->_needs_size_calc = false;
  widget->total_child_size.x = child_size.x;
  widget->total_child_size.y = child_max.y;

  if (fabs(widget->scroll_offset.y) > fabs(widget->container.size.y - widget->total_child_size.y)) {
    widget->scroll_offset.y = 0;
  }
  if (fabs(widget->scroll_offset.x) > fabs(widget->container.size.x - widget->total_child_size.x)) {
    widget->scroll_offset.x = 0;
  }
  mark_widget_and_children_finished(widget);
}

void 
lf_size_calc_responsive_grid(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  (void)widget;
}
