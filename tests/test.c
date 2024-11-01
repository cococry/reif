#include <GLFW/glfw3.h>
#include <leif/color.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/win.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>

lf_div_t* mydiv;

float linear_ease(float t) {
    return t;
}

float ease_out_quad(float t) {
    return 1 - (1 - t) * (1 - t);
}

float ease_in_quad(float t) {
    return t * t;
}

float ease_in_out_quad(float t) {
    return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}

#define ANIM_TIME 0.3
#define ANIM_BEGIN 30.0f
#define ANIM_END 50.0f
#define EASING ease_in_quad 

lf_font_t big_font, small_font;


void widget_store_props(lf_widget_t* widget) {
  widget->_initial_props = widget->props;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    widget_store_props(widget->childs[i]);
  }
}
void widget_restore_props(lf_widget_t* widget) {
  widget->props = widget->_initial_props;
  for(uint32_t i = 0; i < widget->num_childs; i++) {
    widget_restore_props(widget->childs[i]);
  }
}

void on_button_enter(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  float start_val = ANIM_BEGIN; 
  float end_val = ANIM_END;
  float anim_time = ANIM_TIME;
  lf_animation_func_t easing = ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);
  
}

void on_button_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  float start_val = ANIM_END; 
  float end_val = ANIM_BEGIN;
  float anim_time = ANIM_TIME;
  lf_animation_func_t easing = ease_in_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);
}

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(1280, 720, "hello leif");
  
  lf_ui_state_t* ui = lf_ui_core_init(win);
  ui->root->props.color = lf_color_from_hex(0xeeeeee);

  mydiv = lf_div_create(ui, ui->root);
  mydiv->base.props.color = lf_color_from_hex(0x999999);
  mydiv->base.props.corner_radius = 10;


  lf_text_create(ui, &mydiv->base, "Hello, world!");
  lf_button_create_with_label(ui, &mydiv->base, "The white fox!");


  lf_ui_core_submit(ui);

  while(ui->running) {
    lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
