#include <GLFW/glfw3.h>
#include <leif/color.h>
#include <leif/ez_api.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
#include <leif/ui_core.h>
#include <leif/util.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/widgets/text.h>
#include <leif/win.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  lf_text_t* time_text;
  lf_ui_state_t* ui;
  lf_timer_t* tick_timer;
} state_t;

static state_t s;

static lf_font_t heading_chars;


static lf_text_t* text, *heading;

void on_click_left(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(s.ui, &text->base, text_align, ParagraphAlignmentLeft);
  lf_text_set_label(s.ui, heading, "Text is left aligned.");
  lf_ui_core_submit(s.ui);
}

void on_click_center(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(s.ui, &text->base, text_align, ParagraphAlignmentCenter);
  lf_text_set_label(s.ui, heading, "Text is center aligned.");
  lf_ui_core_submit(s.ui);
}

void on_click_right(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(s.ui, &text->base, text_align, ParagraphAlignmentRight);
  lf_text_set_label(s.ui, heading, "Text is right aligned.");
  lf_ui_core_submit(s.ui);
}

#define ANIM_TIME 0.3
#define ANIM_BEGIN 20.0f
#define ANIM_END 25.0f

void button_hover(lf_ui_state_t* ui, lf_widget_t* widget) {

  float start_val = ANIM_BEGIN; 
  float end_val = ANIM_END;
  float anim_time = ANIM_TIME;
  lf_animation_func_t easing = lf_ease_out_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);
}

void on_button_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  (void)widget;
  float start_val = ANIM_END; 
  float end_val = ANIM_BEGIN;
  float anim_time = ANIM_TIME;
  lf_animation_func_t easing = lf_ease_in_quad;
  lf_widget_add_animation(widget, &widget->_initial_props.padding_top, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_bottom, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_left, start_val, end_val, anim_time, easing);
  lf_widget_add_animation(widget, &widget->_initial_props.padding_right, start_val, end_val, anim_time, easing);
}

void button(const char* text, lf_button_func_t on_click) {
  lf_button(s.ui);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;
  ((lf_button_t*)lf_crnt(s.ui))->on_enter = button_hover;
  ((lf_button_t*)lf_crnt(s.ui))->on_leave = on_button_leave;
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x222222));
  lf_style_crnt_widget_prop(s.ui, corner_radius, 10);
  lf_widget_set_fixed_height(lf_crnt(s.ui), 12);
  lf_widget_set_padding(lf_crnt(s.ui), 20);
  lf_style_crnt_widget_prop(s.ui, border_width, 1);
  lf_style_crnt_widget_prop(s.ui, border_color, LF_BLACK);
  lf_text_h3(s.ui, text);
  lf_style_crnt_widget_prop(s.ui, text_color, LF_WHITE);

  lf_button_end(s.ui);
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x282828));

  lf_ui_core_set_font(s.ui, s.ui->render_font_file_from_name("JetBrains Mono Nerd Font"));

  
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical | AlignCenterHorizontal);

  lf_div(s.ui);
  lf_widget_set_margin(lf_crnt(s.ui), 0);
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal);
  heading = lf_text_h1(s.ui, "Text is left aligned."); 
  lf_style_crnt_widget_prop(s.ui, margin_bottom, 30);
  lf_div(s.ui);
  lf_widget_set_max_width(lf_crnt(s.ui), 600);
  lf_widget_set_padding(lf_crnt(s.ui), 20);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x555555));
  lf_style_crnt_widget_prop(s.ui, corner_radius, 15);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal);
  text = lf_text_h3(s.ui, "1. The Industrial Revolution and its consequences have been a disaster for the human race. They have greatly increased the life-expectancy of those of us who live in “advanced” countries, but they have destabilized society, have made life unfulfilling, have subjected human beings to indignities, have led to widespread psychological suffering (in the Third World to physical suffering as well) and have inflicted severe damage on the natural world.");
  lf_style_crnt_widget_prop(s.ui, text_align, ParagraphAlignmentLeft);
  lf_div_end(s.ui);
  lf_div_end(s.ui);

  lf_div(s.ui);
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical);
  button("", on_click_left);
  button("", on_click_center);
  button("", on_click_right);
  lf_div_end(s.ui);
 
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
