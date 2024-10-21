#include <GLFW/glfw3.h>
#include <leif/color.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/win.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>

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
#define ANIM_END 45.0f
#define EASING ease_in_quad 

lf_font_t big_font, small_font;


void on_button_enter(lf_ui_state_t* ui, lf_widget_t* widget) {
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
  (void)ui;
  (void)widget;
  float start_val = ANIM_END; 
  float end_val = ANIM_BEGIN;
  float anim_time = ANIM_TIME;
  lf_animation_func_t easing = ease_in_quad;
  lf_widget_add_animation(widget, &widget->props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->props.padding_right, start_val, end_val, anim_time, easing);
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

  lf_div_t* div = lf_div_create(ui, ui->root);
  div->base.props.color = LF_WHITE;
  lf_div_set_fixed_height(div, lf_win_get_size(ui->win).y);
  lf_widget_set_listener(&div->base, on_resize, WinEventResize);
  lf_widget_set_layout(&div->base, LayoutHorizontal);

  lf_alignment_flag_set(&div->flags, AlignCenterVertical | AlignCenterHorizontal);

  const char* buttons[] = {
    " Shutdown",
    " Reboot",
    " Hold",
  };

  lf_font_t* font = lf_load_font(
    ui, "/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Bold.ttf", 24
  );
  for(uint32_t i = 0; i < 3; i++) {
    lf_button_t* btn = lf_button_create_with_label(ui, &div->base, buttons[i]);
    btn->on_enter = on_button_enter;
    btn->on_leave = on_button_leave;
    btn->base.props.color = lf_color_from_hex(0x111111);
    btn->base.props.border_width = 0.0f;
    lf_widget_set_padding(&btn->base, ANIM_BEGIN);
    lf_button_set_font_size(ui, btn, 30);
    lf_button_set_font(ui, btn, font);
    btn->base.props.corner_radius = 10; 
    btn->text_color = LF_WHITE;
    lf_button_set_fixed_height(btn, 30);
    lf_button_set_fixed_width(btn, 200);
    lf_alignment_flag_set(&btn->flags, AlignCenterVertical);
  }

  lf_ui_core_submit(ui);

  while(ui->running) {
    lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
