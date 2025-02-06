#include "../../include/leif/widgets/button.h"

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

static void _button_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget);

static void _button_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t event);

static void _button_shape(
  lf_ui_state_t* ui, 
  lf_widget_t* widget);

static void _button_size_calc(
  lf_ui_state_t* ui, 
  lf_widget_t* widget);

void
_button_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget) { 
  if(!widget) return;

  lf_button_t* button = (lf_button_t*)widget;
  
  if(button->_held) {
    widget->_rendered_props.color =  lf_color_dim(button->base.props.color, 0.8f);
  } else if(button->_hovered) {
    widget->_rendered_props.color =  lf_color_dim(button->base.props.color, 0.9f);
  } else if(!button->_hovered) {
    widget->_rendered_props.color = button->base.props.color;
  }

  ui->render_rect(
    ui->render_state, 
    widget->container.pos,
    (vec2s){
      .x = widget->container.size.x + widget->_rendered_props.padding_left + widget->_rendered_props.padding_right,
      .y = widget->container.size.y + widget->_rendered_props.padding_top + widget->_rendered_props.padding_bottom
    },
    widget->_rendered_props.color, widget->props.border_color,
    widget->_rendered_props.border_width, widget->props.corner_radius);
}

void 
_button_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(!widget) return;
  if(widget->type != WidgetTypeButton) return;
  lf_widget_apply_layout(ui, widget);
}

void 
_button_size_calc(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(!widget) return;
  if(widget->type != WidgetTypeButton) return;
  lf_widget_calc_layout_size(ui, widget);
}

void 
_button_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t event) {

  if(event.button != LeftMouse && event.type != WinEventMouseMove) return;
  if(!lf_container_intersets_container(
    widget->container, ui->root->container)) {
    return;
  }

  vec2s mouse = (vec2s){
    .x = (float)event.x, 
    .y = (float)event.y}; 

  lf_container_t container = (lf_container_t){
    .pos = widget->container.pos, 
    .size = LF_WIDGET_SIZE_V2(widget)
  };

  lf_button_t* button = (lf_button_t*)widget;
  if(!lf_point_intersets_container(mouse, container)) {
    if(button->_hovered) {
      button->_held = false;
      button->_hovered = false;
      if(button->on_leave) {
        button->on_leave(ui, widget);
      }
      lf_ui_core_rerender_widget(ui, &button->base);
    }
    return;
  }

  if(event.type == WinEventMouseRelease) {
    button->_held = false;
    if(button->on_click) {
      button->on_click(ui, widget);
    }
    lf_ui_core_rerender_widget(ui, &button->base);
    return;
  }
  if(event.type == WinEventMousePress) {
    button->_held = true;
    lf_ui_core_rerender_widget(ui, &button->base);
    return;
  }

  else if(event.type == WinEventMouseMove &&
    !button->_hovered) {
    button->_hovered = true;
    if(button->on_enter) {
      button->on_enter(ui, widget);
    }
    lf_ui_core_rerender_widget(ui, &button->base);
    return;
  }
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


  lf_widget_props_t props = ui->theme->button_props;
  button->base = *lf_widget_create(
    ui->crnt_widget_id++,
    WidgetTypeButton,
    LF_SCALE_CONTAINER(
      150, 
      50),
    props,
    _button_render, 
    _button_handle_event,
    _button_shape,
    _button_size_calc
  );

  button->base.layout_type = LayoutHorizontal;
  lf_widget_set_alignment(&button->base, AlignCenterHorizontal | AlignCenterVertical);
  button->base.sizing_type = SizingFitToContent;

  lf_widget_listen_for(&button->base, 
                       WinEventMouseRelease | WinEventMousePress | WinEventMouseMove);
  
  lf_widget_add_child(parent, (lf_widget_t*)button);

  return button;
}
