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


void on_button_enter(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  float start_val = ANIM_BEGIN; 
  float end_val = ANIM_END;
  float anim_time = ANIM_TIME;
  lf_animation_func_t easing = ease_out_quad;
  lf_widget_add_animation(widget, &widget->props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_right, start_val, end_val, anim_time, easing);
}

void on_button_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  float start_val = ANIM_END; 
  float end_val = ANIM_BEGIN;
  float anim_time = ANIM_TIME;
  lf_animation_func_t easing = ease_in_quad;
  lf_widget_add_animation(widget, &widget->props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_right, start_val, end_val, anim_time, easing);
}

void on_button_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  lf_widget_set_layout(widget->parent, widget->parent->layout_type == LayoutGrid ?  LayoutHorizontal : LayoutGrid);
  lf_widget_interrupt_all_animations(widget);
  lf_ui_core_submit(ui);
}

void on_resize(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t ev) {
  if(widget->type != WidgetTypeDiv) return;
  lf_div_t* div = (lf_div_t*)widget;
  lf_div_set_fixed_height(div, ev.height);
  lf_ui_core_submit(ui);
}

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(1280, 720, "hello leif");
  
  lf_ui_state_t* ui = lf_ui_core_init(win);
  ui->root->props.color = lf_color_from_hex(0x00ff00);

  lf_div_t* div = lf_div_create(ui, ui->root);
  lf_widget_set_padding(&div->base, 0);
  lf_widget_set_margin(&div->base, 0);
  div->base.props.color = lf_color_from_hex(0xff0000);

  lf_widget_set_layout(&div->base, LayoutGrid);

  lf_div_set_fixed_height(div, 500);
  lf_div_set_column_count(div, 3);

  for(uint32_t i = 0; i < 3*3; i++) {
    char buf[64];
    sprintf(buf, "Привет, мир: %i", i * 100);
    lf_button_t* btn = lf_button_create_with_label(ui, &div->base, buf);
    btn->on_enter = on_button_enter;
    btn->on_leave = on_button_leave;
    btn->on_click = on_button_click;
    lf_widget_set_padding(&btn->base, ANIM_BEGIN);
    lf_widget_set_margin(&btn->base, 20);
    btn->base.props.corner_radius = 20;
  }


  lf_ui_core_submit(ui);

  while(ui->running) {
    lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
