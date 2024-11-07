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

typedef struct {
  int32_t counter;
  lf_text_t* text;
} state_t;

static state_t s;

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  if(((lf_button_t*)widget)->label[0] == 'I')
    s.counter++;
  else 
    s.counter--;
  char buf[32];
  sprintf(buf,"Counter: %i", s.counter);
  lf_text_set_label(ui, s.text, buf);
  lf_ui_core_submit(ui);
}


void on_button_enter(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  float start_val = 10.0f; 
  float end_val = 15.0f;
  float anim_time = 0.2f;
  lf_animation_func_t easing = lf_ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);
}

void on_button_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  (void)ui;
  (void)ui;
  float start_val = 15.0f; 
  float end_val = 10.0f;
  float anim_time = 0.2f;
  lf_animation_func_t easing = lf_ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);
}

int main(void) {
  s.counter = 0;

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(1280, 720, "hello leif");
  lf_ui_state_t* ui = lf_ui_core_init(win);

  for(uint32_t i = 0; i < 2; i++) {
    lf_div(ui);

    lf_style_crnt_widget_prop(ui, color, lf_color_from_hex(0x555555));
    lf_crnt(ui)->sizing_type = SizingFitToContent;

    lf_text_h1(ui, "Hello, World!");
    lf_text_h2(ui, "Привет, мир!"); // Russian
    lf_text_h3(ui, "Hallo, Welt!"); // German
    lf_text_h4(ui, "Witaj, świecie!"); // Polish
    lf_text_h5(ui, "Hei, verden!"); // Norwegian
    lf_text_h6(ui, "Γειά σου, Κόσμε!"); // Greek
    lf_div_end(ui);
  }

  lf_ui_core_submit(ui);

  while(ui->running) {
    lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
