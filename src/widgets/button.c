#include "../../include/leif/widgets/button.h"

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

static void _button_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t* event);

static void _button_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget);

static void _button_shape(
  lf_ui_state_t* ui, 
  lf_widget_t* widget);

static void _button_size_calc(
  lf_ui_state_t* ui, 
  lf_widget_t* widget);

static void _reset_buttons(
  lf_ui_state_t* ui, 
  lf_widget_t* widget,
  lf_widget_t* skip
) {
  if(widget != skip && widget->type == LF_WIDGET_TYPE_BUTTON) {
    lf_button_t* btn = ((lf_button_t*)widget);
    if(btn->_was_hovered)
      lf_widget_set_props(ui, widget, btn->_before_hovered_props);
  }
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    _reset_buttons(ui, widget->childs[i], skip);
  }
}
void 
_button_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t* event) {
  if(!lf_container_intersets_container(
    widget->container, ui->root->container)) {
    return;
  }

  vec2s mouse = (vec2s){
    .x = (float)event->x, 
    .y = (float)event->y}; 

  lf_container_t container = (lf_container_t){
    .pos = widget->container.pos, 
    .size = LF_WIDGET_SIZE_V2(widget)
  };

  lf_button_t* button = (lf_button_t*)widget;
  bool on_button = lf_point_intersets_container(mouse, container);
 if(!on_button && !button->_hovered && event->type == LF_EVENT_MOUSE_MOVE && !lf_widget_props_equal(widget->props, widget->_initial_props) && button->_was_hovered) {
    _reset_buttons(ui, ui->root, widget);
    lf_widget_set_props(ui, widget, button->_before_hovered_props);
    ui->needs_render = true;
    button->_was_hovered = false;
  }
  if(!on_button && event->type == LF_EVENT_MOUSE_MOVE &&  !button->_held && button->_hovered) {
    button->_held = false;
    button->_hovered = false;
    ui->needs_render = true;
    _reset_buttons(ui, ui->root, widget);
    lf_widget_set_props(ui, widget, button->_before_hovered_props);

    lf_win_set_cursor(ui->win, LF_CURSOR_ARROW); 
    if(button->on_leave) {
      button->on_leave(ui, widget);
    }
    return;
  }
 if(on_button && event->type == LF_EVENT_MOUSE_MOVE &&
    !button->_hovered && ui->active_widget_id == 0) {
    button->_hovered = true;
    button->_before_hovered_props = widget->_rendered_props; 
    lf_widget_set_props(ui, widget, button->hovered_props);
    ui->needs_render = true;
    lf_win_set_cursor(ui->win, LF_CURSOR_ARROW);
    if(button->on_enter) {
      button->on_enter(ui, widget);
    }
    button->_was_hovered = true;
    return;
  }

  if(event->type == LF_EVENT_MOUSE_RELEASE && button->_held) {
    button->_held = false;
    lf_widget_props_t clicked_props = button->hovered_props;
    if(on_button) {
    clicked_props.color = lf_color_dim(button->hovered_props.color, 90.0f);
    } else {
      clicked_props = widget->_component_props; 
    }
    lf_widget_set_props(ui, widget, clicked_props);
    ui->needs_render = true;
    if(widget->id == ui->active_widget_id) {
      if(button->on_click && on_button) {
        button->on_click(ui, widget);
      }
    }
    ui->active_widget_id = 0;
    return;
  }
  if(event->type == LF_EVENT_MOUSE_PRESS && on_button) {
    lf_widget_props_t held_props = button->hovered_props;
    held_props.color = lf_color_dim(button->hovered_props.color, 80.0f);
    lf_widget_set_props(ui, widget, held_props);
    button->_held = true;
    ui->active_widget_id = widget->id;
    ui->needs_render = true;
    return;
  }
}


void
_button_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget) { 
  if(!widget) return;

  ui->render_rect(
    ui->render_state, 
    widget->container.pos,
    (vec2s){
      .x = widget->container.size.x + widget->props.padding_left + widget->props.padding_right,
      .y = widget->container.size.y + widget->props.padding_top + widget->props.padding_bottom
    },
    widget->props.color, widget->props.border_color,
    widget->props.border_width, widget->props.corner_radius);
}

void 
_button_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(!widget) return;
  if(widget->type != LF_WIDGET_TYPE_BUTTON) return;
  lf_widget_apply_layout(ui, widget);
}

void 
_button_size_calc(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(!widget) return;
  if(widget->type != LF_WIDGET_TYPE_BUTTON) return;
  lf_widget_calc_layout_size(ui, widget);
}
  
lf_button_t* 
lf_button_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent) {
  lf_button_t* button = (lf_button_t*)malloc(sizeof(lf_button_t));
  button->on_click = NULL;
  button->on_enter = NULL;
  button->on_leave = NULL;
  button->_hovered = false;
  button->_held = false;
  button->_was_hovered = false;

  lf_widget_props_t props = ui->theme->button_props;
  button->base = *lf_widget_create(
    ui->crnt_widget_id++,
    LF_WIDGET_TYPE_BUTTON,
    LF_SCALE_CONTAINER(
      0, 
      0),
    props,
    _button_render, 
    _button_handle_event,
    _button_shape,
    _button_size_calc
  );
  button->hovered_props = props;
  button->hovered_props.color = 
    lf_color_dim(button->base._initial_props.color, 90.0f);

  button->base.layout_type = LF_LAYOUT_HORIZONTAL;
  lf_widget_set_alignment(&button->base, LF_ALIGN_CENTER_VERTICAL | LF_ALIGN_CENTER_HORIZONTAL);
  button->base.sizing_type = LF_SIZING_FIT_CONTENT;

  lf_widget_listen_for(&button->base, 
                       LF_EVENT_MOUSE_PRESS | LF_EVENT_MOUSE_RELEASE | LF_EVENT_MOUSE_MOVE);
  
  lf_widget_add_child(parent, (lf_widget_t*)button);

  return button;
}
