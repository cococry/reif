#include "../../include/leif/widgets/div.h"
#include "../../include/leif/util.h"
#include "../../include/leif/layout.h"

static void _div_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t* event);
static void _div_render(lf_ui_state_t* ui, lf_widget_t* widget);
static void _div_shape(lf_ui_state_t* ui, lf_widget_t* widget);
static void _div_calc_size(lf_ui_state_t* ui, lf_widget_t* widget);

void 
_div_handle_event(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t* event) {
  printf("Handled.\n");
  if(!lf_container_intersets_container(
    widget->container, ui->root->container)) {
    return;
  }

  vec2s mouse = (vec2s){
    .x = (float)event->x, 
    .y = (float)event->y};

  lf_container_t div_container = (lf_container_t){
    .pos = widget->container.pos, 
    .size = LF_WIDGET_SIZE_V2(widget)
  };
  lf_div_t* div = (lf_div_t*)widget;
  if(event->type == WinEventMouseWheel && lf_point_intersets_container(mouse, div_container)) {
    if(!(event->scroll_x != 0 || event->scroll_y != 0)) return; 
    float total_scrollable_area = 
      ((event->scroll_x != 0) ? widget->total_child_size.x : widget->total_child_size.y) -
      ((event->scroll_x != 0) ? widget->container.size.x : widget->container.size.y); 
    if (total_scrollable_area > 0) {
      float scroll = (event->scroll_x != 0) ? event->scroll_x : event->scroll_y;
      float scroll_end = 
        ((event->scroll_x != 0) ? widget->scroll_offset.x : widget->scroll_offset.y) + scroll * 60;

      if(scroll_end > 0) scroll_end = 0;
      if(scroll_end < -total_scrollable_area) scroll_end = -total_scrollable_area;

      lf_widget_add_animation(
        widget, 
        (event->scroll_x != 0.0f) ? &widget->scroll_offset.x : &widget->scroll_offset.y, 
        (event->scroll_x != 0.0f) ? widget->scroll_offset.x : widget->scroll_offset.y, 
        scroll_end, 0.075f, lf_ease_out_quad);
      div->_last_scroll_end = scroll_end;
      
      lf_widget_invalidate_layout(widget);
      lf_widget_shape(ui, widget);
      ui->needs_render = true;
      event->handled = true;
    }
  }

  for(uint32_t i = 0; i < LF_SCROLLBAR_COUNT; i++) {
    lf_scrollbar_t* scrollbar = &div->scrollbars[i];
    lf_container_t container = scrollbar->container;

    bool on_scrollbar = lf_point_intersets_container(mouse, container);
    if(!on_scrollbar && !scrollbar->held && scrollbar->hovered) {
      scrollbar->held = false;
      scrollbar->hovered = false;
      ui->needs_render = true;
      lf_widget_set_prop_color(
        ui, widget, 
        &scrollbar->color, 
        ui->theme->scrollbar_props.color);
    }

    if(event->type == WinEventMouseRelease && scrollbar->held) {
      scrollbar->held = false;
      ui->active_widget_id = 0;
      scrollbar->hovered = false;
      lf_widget_set_prop_color(
        ui, widget, 
        &scrollbar->color, 
        ui->theme->scrollbar_props.color);
      ui->needs_render = true;
    }
    if(event->type == WinEventMousePress && lf_point_intersets_container(mouse, container)) {
      scrollbar->held = true;
      ui->active_widget_id = widget->id;
      ui->needs_render = true;
      div->_scrollbar_drag_start = mouse;
      div->_scroll_offset_start = widget->scroll_offset;
      lf_widget_set_prop_color(
        ui, widget,
        &scrollbar->color, 
        lf_color_dim(ui->theme->scrollbar_props.color, 0.8f));
    }

    else if(on_scrollbar && (event->type == WinEventMouseMove || event->type == WinEventMouseWheel) &&
      !scrollbar->hovered && ui->active_widget_id == 0) {
      scrollbar->hovered = true;
      lf_widget_set_prop_color(
        ui, widget, 
        &scrollbar->color, 
        lf_color_dim(ui->theme->scrollbar_props.color, 0.9f));
      ui->needs_render = true;
    }
    else if (event->type == WinEventMouseMove && scrollbar->held) {
      if(i == LF_SCROLLBAR_VERTICAL) {
        float total_scrollable_area = widget->total_child_size.y - widget->container.size.y;
        float total_scrollbar_movable_area = widget->container.size.y - 
          (widget->container.size.y / widget->total_child_size.y) * widget->container.size.y;

        if (total_scrollable_area > 0) {
          float delta_mouse = event->y - div->_scrollbar_drag_start.y;  
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
      else {
        float total_scrollable_area = widget->total_child_size.x - widget->container.size.x;
        float total_scrollbar_movable_area = widget->container.size.x - 
          (widget->container.size.x / widget->total_child_size.x) * widget->container.size.x;

        if (total_scrollable_area > 0) {
          float delta_mouse = event->x - div->_scrollbar_drag_start.x;  
          float scroll_ratio = total_scrollable_area / total_scrollbar_movable_area; 

          widget->scroll_offset.x = div->_scroll_offset_start.x - (delta_mouse * scroll_ratio);
          if (widget->scroll_offset.x >= 0.0f) {
            widget->scroll_offset.x = 0.0f;
          }
          if (widget->scroll_offset.x <= -total_scrollable_area) {
            widget->scroll_offset.x = -total_scrollable_area;
          }

          lf_widget_invalidate_layout(widget);
          lf_widget_shape(ui, widget);
          ui->needs_render = true;
        }
      }
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
  memset(div->scrollbars, 0, sizeof(div->scrollbars));
  for(uint32_t i = 0; i < LF_SCROLLBAR_COUNT; i++) {
    div->scrollbars[i].color = ui->theme->scrollbar_props.color;
  }
  div->_scrollbar_drag_start = (vec2s){.x = 0,.y = 0};
  div->_scroll_offset_start = (vec2s){.x = 0, .y = 0};
  div->_scroll_velocity = 0.0f;
  div->_last_scroll_end = 0.0f;

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
                       WinEventMouseRelease | WinEventMousePress | WinEventMouseMove | WinEventMouseWheel);

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
