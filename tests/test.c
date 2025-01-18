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

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  (void)ui;
  printf("Clicked button!\n");
}


static lf_text_t* text;

void on_click_left(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(s.ui, &text->base, text_align, ParagraphAlignmentLeft);
  lf_ui_core_submit(s.ui);
}

void on_click_center(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(s.ui, &text->base, text_align, ParagraphAlignmentCenter);
  lf_ui_core_submit(s.ui);
}

void on_click_right(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(s.ui, &text->base, text_align, ParagraphAlignmentRight);
  lf_ui_core_submit(s.ui);
}

void button(const char* text, lf_button_func_t on_click) {
  lf_button(s.ui);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;
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

  
  lf_widget_set_padding(lf_crnt(s.ui), 20);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical | AlignCenterHorizontal);

  lf_div(s.ui);
  lf_widget_set_max_width(lf_crnt(s.ui), 600);
  lf_widget_set_padding(lf_crnt(s.ui), 20);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x555555));
  lf_style_crnt_widget_prop(s.ui, corner_radius, 15);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal);
  text = lf_text_h4(s.ui, "1. The Industrial Revolution and its consequences have been a disaster for the human race. They have greatly increased the life-expectancy of those of us who live in “advanced” countries, but they have destabilized society, have made life unfulfilling, have subjected human beings to indignities, have led to widespread psychological suffering (in the Third World to physical suffering as well) and have inflicted severe damage on the natural world.");
  lf_style_crnt_widget_prop(s.ui, text_align, ParagraphAlignmentLeft);
  lf_div_end(s.ui);

  lf_div(s.ui);
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
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
