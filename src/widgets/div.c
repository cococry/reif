#include "../../include/leif/widgets/div.h"
#include "../../include/leif/util.h"
#include "../../include/leif/layout.h"

static void _div_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t event);
static void _div_render(lf_ui_state_t* ui, lf_widget_t* widget);
static void _div_shape(lf_ui_state_t* ui, lf_widget_t* widget);
static void _div_calc_size(lf_ui_state_t* ui, lf_widget_t* widget);

void 
_div_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t event) {
 if(event.button != LeftMouse) return;
  if(!lf_container_intersets_container(
    widget->container, ui->root->container)) {
    return;
  }

  vec2s mouse = (vec2s){
    .x = (float)event.x, 
    .y = (float)event.y};

  lf_div_t* div = (lf_div_t*)widget;
  lf_container_t container = div->_scrollbar_container;

  if(!lf_point_intersets_container(mouse, container) && !div->_held_scrollbar) {
    if(div->_hovered_scrollbar) {
      div->_held_scrollbar = false;
      div->_hovered_scrollbar = false;
      ui->needs_render = true;
      lf_widget_set_prop_color(
        ui, widget, 
        &div->_scrollbar_color, 
        ui->theme->scrollbar_props.color);
    }
    return;
  }

  if(event.type == WinEventMouseRelease && div->_held_scrollbar) {
    div->_held_scrollbar = false;
      div->_hovered_scrollbar = false;
    lf_widget_set_prop_color(
      ui, widget, 
      &div->_scrollbar_color, 
      ui->theme->scrollbar_props.color);
    ui->needs_render = true;
    return;
  }
  if(event.type == WinEventMousePress) {
    div->_held_scrollbar = true;
    ui->needs_render = true;
    div->_scrollbar_drag_start = mouse;
    div->_scroll_offset_start = widget->scroll_offset;
    lf_widget_set_prop_color(
      ui, widget,
      &div->_scrollbar_color, 
      lf_color_dim(ui->theme->scrollbar_props.color, 0.8f));
    return;
  }

  else if(event.type == WinEventMouseMove &&
    !div->_hovered_scrollbar) {
    div->_hovered_scrollbar = true;
    lf_widget_set_prop_color(
      ui, widget, 
      &div->_scrollbar_color, 
      lf_color_dim(ui->theme->scrollbar_props.color, 0.9f));
    ui->needs_render = true;
    return;
  }
  else if (event.type == WinEventMouseMove && div->_held_scrollbar) {
    float total_scrollable_area = widget->total_child_size.y - widget->container.size.y;
    float total_scrollbar_movable_area = widget->container.size.y - 
      (widget->container.size.y / widget->total_child_size.y) * widget->container.size.y;

    if (total_scrollable_area > 0) {
      float delta_mouse = event.y - div->_scrollbar_drag_start.y;  
      float scroll_ratio = total_scrollable_area / total_scrollbar_movable_area; 

      widget->scroll_offset.y = div->_scroll_offset_start.y - (delta_mouse * scroll_ratio);
      if (widget->scroll_offset.y >= 0.0f) {
        widget->scroll_offset.y = 0.0f;
      }
      if (widget->scroll_offset.y <= -total_scrollable_area) {
        widget->scroll_offset.y = -total_scrollable_area;
      }

      lf_widget_invalidate_layout(widget);
      lf_widget_shape(ui, widget);
      ui->needs_render = true;
    }
  }

}

void 
_div_render(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_div_t* div = (lf_div_t*)widget;
  ui->render_rect(
    ui->render_state, div->base.container.pos,
    LF_WIDGET_SIZE_V2(widget),
    widget->props.color, widget->props.border_color, 
    widget->props.border_width, widget->props.corner_radius);
 }

void 
_div_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  if(widget->type != WidgetTypeDiv) return;
  lf_widget_apply_layout(ui, widget);
}

void 
_div_calc_size(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  if(widget->type != WidgetTypeDiv) return;
  lf_widget_calc_layout_size(ui, widget);
}

lf_div_t* 
lf_div_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent) {
  if(!parent) return NULL;

  lf_div_t* div = malloc(sizeof(*div));
  div->_held_scrollbar = false;
  div->_scrollbar_color = ui->theme->scrollbar_props.color;
  div->_scrollbar_drag_start = (vec2s){.x = 0,.y = 0};
  div->_scroll_offset_start = (vec2s){.x = 0, .y = 0} ;

  div->base = *lf_widget_create(
    ui->crnt_widget_id++,
    WidgetTypeDiv,
    LF_SCALE_CONTAINER(parent->container.size.x,0),
    ui->theme->div_props,
    _div_render,
    _div_handle_event,
    _div_shape,
    _div_calc_size
    );

  lf_widget_listen_for(&div->base, 
                       WinEventMouseRelease | WinEventMousePress | WinEventMouseMove);

  ui->crnt_widget_id++;

  lf_widget_add_child(parent, (lf_widget_t*)div);

  div->base.layout_type = LayoutVertical;
  div->_column_count = 0;

  return div;
}

void lf_div_set_column_count(
    lf_div_t* div,
    uint32_t column_count) {
  div->_column_count = (int32_t)column_count;
}
