#include "../../include/leif/widgets/button.h"
#include "../../include/leif/ui_core.h"
#include "../../include/leif/event.h"
#include "../../include/leif/layout.h"
#include "../../include/leif/render.h"
#include "../../include/leif/util.h"
#include <cglm/types-struct.h>
#include <string.h>

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

void
_button_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget) { 
  if(!widget) return;

  lf_button_t* button = (lf_button_t*)widget;
 
  ui->render_rect(
    ui->render_state, 
    widget->container.pos, 
    LF_WIDGET_SIZE_V2(widget),
    widget->props.color, widget->props.border_color,
    widget->props.border_width, widget->props.corner_radius);

  if(button->label) {
    vec2s text_pos =(vec2s){
      .x = widget->container.pos.x + widget->props.padding_left, 
      .y =  widget->container.pos.y + widget->props.padding_top 
    };

    if(button->_fixed_width && button->centered_text)
      text_pos.x = widget->container.pos.x + (lf_widget_width(widget) - button->_text_dimension.width) / 2.0f;
    if(button->_fixed_height && button->centered_text)
      text_pos.y = widget->container.pos.y + (lf_widget_height(widget) - button->_text_dimension.height) / 2.0f;

    ui->render_text(
      ui->render_state,
      button->label,
      button->font,
      text_pos,
      button->text_color
    );
  }
}

void 
_button_handle_event(
  lf_ui_state_t* ui, 
  lf_widget_t* widget, 
  lf_event_t event) {
  (void)ui;
  if(event.button != LeftMouse) return;
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
    if(button->_hovered && button->on_leave) {
      button->on_leave(ui, widget);
    }
    button->_hovered = false;
    return;
  }

  if(event.type == WinEventMouseRelease && 
    button->on_click) {
      button->on_click(ui, widget);
  }
  else if(event.type == WinEventMouseMove &&
    button->on_enter && !button->_hovered) {
    button->on_enter(ui, widget);
    button->_hovered = true;
  }
}
  
lf_button_t* 
lf_button_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent) {

  lf_button_t* button = (lf_button_t*)malloc(sizeof(lf_button_t));

  button->base = *lf_widget_create(
    WidgetTypeButton,
    LF_SCALE_CONTAINER(100.0f, ui->render_font_get_size(ui->font_p)), 
    ui->theme->button_props,
    _button_render, 
    _button_handle_event,
    NULL
  );

  button->label = NULL;
  button->font = ui->font_p;
  button->text_color = ui->theme->text_color;
  button->on_click = NULL;
  button->on_enter = NULL;
  button->on_leave = NULL;
  button->_changed_font_size = false;
  button->_hovered = false;
  button->_text_dimension = (lf_text_dimension_t){.width = 0, .height = 0};
  button->centered_text = false;

  button->base.layout_type = LayoutNone;
  lf_widget_add_child(parent, (lf_widget_t*)button);

  return button;
}

lf_button_t*
lf_button_create_with_label(
  lf_ui_state_t* ui,
  lf_widget_t* parent,
  const char* label) {
  return lf_button_create_with_label_ex(
    ui,
    parent,
    label,
    ui->font_p);
}


lf_button_t* 
lf_button_create_with_label_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label,
    lf_font_t font) {

  lf_button_t* button = (lf_button_t*)malloc(sizeof(lf_button_t));
  button->label = strdup(label);
  button->font = font; 
  button->on_click = NULL;
  button->on_enter = NULL;
  button->on_leave = NULL;
  button->text_color = ui->theme->text_color;
  button->_changed_font_size = false;
  button->_hovered = false;

  button->_fixed_width = false;
  button->_fixed_height = false;
  button->centered_text = false;

  lf_text_dimension_t text_dimension = ui->render_get_text_dimension(
    ui->render_state,
    button->label,
    button->font
  );

  button->_text_dimension = text_dimension;

  lf_widget_props_t props = ui->theme->button_props;
  button->base = *lf_widget_create(
    WidgetTypeButton,
    LF_SCALE_CONTAINER(
      text_dimension.width, 
      text_dimension.height),
    props,
    _button_render, 
    _button_handle_event,
    NULL
  );
  button->base.layout_type = LayoutNone;

  lf_widget_listen_for(&button->base, 
                       WinEventMouseRelease | WinEventMouseMove);
  
  lf_widget_add_child(parent, (lf_widget_t*)button);

  return button;
}

void lf_button_set_font(
    lf_ui_state_t* ui, 
    lf_button_t* button,
    void* font) {
  button->font = font;
  lf_text_dimension_t text_dimension = ui->render_get_text_dimension(
    ui->render_state,
    button->label,
    button->font
  );
  button->base.container.size.x = text_dimension.width;
  button->base.container.size.y = text_dimension.height;

  button->_text_dimension = text_dimension;
}


void 
lf_button_set_font_size(
    lf_ui_state_t* ui, 
    lf_button_t* button,
    uint32_t size) {
#ifdef LF_RUNARA
  if(!button->_changed_font_size) {
    button->font = lf_load_font(ui, ((RnFont*)button->font)->filepath, size);
  } else {
    lf_font_resize(ui, button->font, size);
  }
  lf_button_set_font(ui, button, button->font);
#endif
}

void lf_button_set_fixed_width(
    lf_button_t* button,
    float width) {
  button->base.container.size.x = width;
  button->_fixed_width = true;
}

void lf_button_set_fixed_height(
    lf_button_t* button,
    float height) {
  button->base.container.size.y = height;
  button->_fixed_height = true;
}
