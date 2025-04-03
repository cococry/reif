
#include "../../include/leif/widgets/slider.h"

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

static void _slider_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t* event);

static void _slider_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget);

static void _slider_shape(
  lf_ui_state_t* ui,
  lf_widget_t* widget);

static void _slider_handle_mouse(
  vec2s mouse, lf_widget_t* widget, 
  lf_slider_t* slider);

void 
_slider_handle_mouse(
  vec2s mouse, lf_widget_t* widget, 
  lf_slider_t* slider) {
    *slider->val = slider->min + ((mouse.x - widget->container.pos.x) / widget->container.size.x) * (slider->max - slider->min);
    if(*slider->val > slider->max) *slider->val = slider->max;
    if(*slider->val < slider->min) *slider->val = slider->min;

    slider->handle.pos = (vec2s){
      .x = slider->handle.pos.x = widget->container.pos.x + 
      (widget->container.size.x * (*slider->val / (slider->max - slider->min)))
    - slider->handle.size.x / 2.0f,
      .y = widget->container.pos.y +
      (lf_widget_height(widget) - 
      (slider->handle.size.y + 
      slider->handle_props.padding_top + 
      slider->handle_props.padding_bottom)) / 2.0f
    };
    if(slider->handle.pos.x >= widget->container.pos.x + widget->container.size.x
      - slider->handle.size.x) {
      slider->handle.pos.x = widget->container.pos.x + widget->container.size.x - slider->handle.size.x;
    }
}

void 
_slider_handle_event(
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

  lf_slider_t* slider = (lf_slider_t*)widget;
  lf_container_t handle = slider->handle; 
  lf_container_t slider_container = (lf_container_t){
    .pos = widget->container.pos,
    .size = LF_WIDGET_SIZE_V2(widget) 
  };

  bool on_handle = lf_point_intersets_container(mouse, handle);
  
  bool on_slider = lf_point_intersets_container(mouse, slider_container);

  if(event->type == LF_EVENT_MOUSE_PRESS && on_slider) {
    _slider_handle_mouse(mouse, widget, slider);
    slider->_held = true;
    ui->active_widget_id = widget->id;
    ui->needs_render = true;
    lf_widget_set_prop_color(
      ui, widget,
      &slider->handle_props.color, 
      lf_color_dim(slider->_initial_handle_props.color, 80.0f));
  }
  if(!on_handle && event->type == LF_EVENT_MOUSE_MOVE &&  !slider->_held && slider->_hovered) {
    slider->_held = false;
    slider->_hovered = false;
    ui->needs_render = true;
    lf_widget_set_prop_color(
      ui, widget, 
      &widget->props.color, 
      slider->base._initial_props.color);

    if(slider->on_leave) {
      slider->on_leave(ui, widget);
    }
    return;
  }
 if(on_handle && event->type == LF_EVENT_MOUSE_MOVE &&
    !slider->_hovered && ui->active_widget_id == 0) {
    slider->_hovered = true;
    lf_widget_set_prop_color(
      ui, widget, 
      &slider->handle_props.color, 
      lf_color_dim(slider->_initial_handle_props.color, 90.0f));
    ui->needs_render = true;
    if(slider->on_enter) {
      slider->on_enter(ui, widget);
    }
    return;
  }

  if(event->type == LF_EVENT_MOUSE_RELEASE && slider->_held) {
    slider->_held = false;
      lf_widget_set_prop_color(
        ui, widget, 
        &slider->handle_props.color, 
        slider->_initial_handle_props.color);
    ui->needs_render = true;
    if(widget->id == ui->active_widget_id) {
      if(slider->on_click && on_handle) {
        slider->on_click(ui, widget);
      }
    }
    ui->active_widget_id = 0;
    return;
  }
  if(event->type == LF_EVENT_MOUSE_PRESS && on_handle) {
    slider->_held = true;
    ui->active_widget_id = widget->id;
    ui->needs_render = true;
    lf_widget_set_prop_color(
      ui, widget,
      &slider->handle_props.color, 
      lf_color_dim(slider->_initial_handle_props.color, 80.0f));
    return;
  }
  if(event->type == LF_EVENT_MOUSE_MOVE && slider->_held) {
    ui->needs_render = true;
    _slider_handle_mouse(mouse, widget, slider);
    if(slider->on_slide)
      slider->on_slide(ui, widget, slider->val);
    return;
  }
}


void
_slider_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget) { 
  if(!widget) return;

  lf_slider_t* slider = (lf_slider_t*)widget;

  ui->render_rect(
    ui->render_state, 
    widget->container.pos,
    (vec2s){
      .x = widget->container.size.x + widget->props.padding_left + widget->props.padding_right,
      .y = widget->container.size.y + widget->props.padding_top + widget->props.padding_bottom
    },
    widget->_rendered_props.color, widget->props.border_color,
    widget->props.border_width, widget->props.corner_radius);

  ui->render_rect(
    ui->render_state, 
    widget->container.pos,
    (vec2s){
      .x = (slider->handle.pos.x - widget->container.pos.x) + widget->props.corner_radius,
      .y = widget->container.size.y + widget->props.padding_top + widget->props.padding_bottom
    },
    widget->props.text_color, LF_NO_COLOR, 0, widget->props.corner_radius);


  ui->render_rect(
    ui->render_state, 
    slider->handle.pos,
    (vec2s){
      .x = slider->handle.size.x + slider->handle_props.padding_left + slider->handle_props.padding_right,
      .y = slider->handle.size.y + slider->handle_props.padding_top + slider->handle_props.padding_bottom
    },
    slider->handle_props.color, slider->handle_props.border_color,
    slider->handle_props.border_width, slider->handle_props.corner_radius);
}

void 
_slider_shape(
  lf_ui_state_t* ui,
  lf_widget_t* widget) {
  lf_slider_t* slider = (lf_slider_t*)widget;
  slider->handle.pos = (vec2s){
      .x = slider->handle.pos.x = widget->container.pos.x + 
      (widget->container.size.x * (*slider->val / (slider->max - slider->min)))
    - slider->handle.size.x / 2.0f,
      .y = widget->container.pos.y +
      (lf_widget_height(widget) - 
      (slider->handle.size.y + 
      slider->handle_props.padding_top + 
      slider->handle_props.padding_bottom)) / 2.0f
    };
    if(slider->handle.pos.x >= widget->container.pos.x + widget->container.size.x
      - slider->handle.size.x) {
      slider->handle.pos.x = widget->container.pos.x + widget->container.size.x - slider->handle.size.x;
    }

}
  
lf_slider_t* 
lf_slider_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent,
  float* val, 
  float min, float max) {
  lf_slider_t* slider = (lf_slider_t*)malloc(sizeof(lf_slider_t));
  slider->on_click = NULL;
  slider->on_enter = NULL;
  slider->on_leave = NULL;
  slider->on_slide = NULL;
  slider->_hovered = false;
  slider->_held = false;

  slider->val = val;
  slider->min = min;
  slider->max = max;


  lf_widget_props_t props = ui->theme->slider_props;
  slider->base = *lf_widget_create(
    ui->crnt_widget_id++,
    LF_WIDGET_TYPE_SLIDER,
    LF_SCALE_CONTAINER(
      150, 
      5),
    props,
    _slider_render, 
    _slider_handle_event,
    _slider_shape,
    NULL 
  );

  slider->handle_props = (lf_widget_props_t){
    .color = lf_color_from_hex(0x555555),
    .padding_left   = 0.0f,
    .padding_right  = 0.0f,
    .padding_top    = 0.0f,
    .padding_bottom = 0.0f,
    .margin_left    = 0.0f,
    .margin_right   = 0.0f,
    .margin_top     = 0.0f,
    .margin_bottom  = 0.0f,
    .corner_radius_percent = 50.0f, 
    .border_width   = 2.0f, 
    .border_color   = LF_WHITE,
  };

  slider->handle.size = (vec2s){.x = 20, .y = 20};
    slider->handle_props.corner_radius =  
    slider->handle.size.y * (slider->handle_props.corner_radius_percent / 100.0f);

  slider->_initial_handle_props = slider->handle_props; 


  slider->base.layout_type = LF_LAYOUT_NONE;

  lf_widget_listen_for(&slider->base, 
                       LF_EVENT_MOUSE_RELEASE | LF_EVENT_MOUSE_PRESS | LF_EVENT_MOUSE_MOVE);
  
  lf_widget_add_child(parent, (lf_widget_t*)slider);

  return slider;
}
