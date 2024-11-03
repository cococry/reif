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

  lf_div_t* div = lf_div_create(ui, ui->root);
  lf_widget_submit_props(&div->base);

  
  lf_font_t font = lf_load_font_from_name(ui, "Inter", 24);

  char buf[32];
  sprintf(buf,"Counter: %i", s.counter);
  s.text = lf_text_create_ex(ui, &div->base, buf, font);

  lf_div_t* div2 = lf_div_create(ui, &div->base);
  div2->base.props.padding_left = 20;
  div2->base.props.padding_right = 20;
  div2->base.props.margin_top = 30;
  div2->base.props.color = lf_color_from_hex(0x999999);

  lf_alignment_flag_set(&div2->base.alignment_flags, AlignCenterVertical);
  lf_widget_set_layout(&div2->base, LayoutResponsiveGrid);
  lf_widget_submit_props(&div2->base);

  lf_div_set_column_count(div2, 2);

  const char* texts[4] = {
    "Increment",
    "Decrement",
    "Decrement",
    "Increment"
  };
  for(uint32_t i = 0; i < 4; i++) {
    lf_button_t* button_inc = lf_button_create_with_label_ex(
      ui, &div2->base,
      texts[i],
      font
    );
    lf_widget_submit_props(&button_inc->base);

    lf_alignment_flag_set(&button_inc->base.alignment_flags, AlignCenterHorizontal);

    button_inc->on_click = on_click;
    button_inc->on_enter = on_button_enter;
    button_inc->on_leave = on_button_leave;
  }
  lf_ui_core_submit(ui);

  while(ui->running) {
    lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
