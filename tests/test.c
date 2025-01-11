#include <GLFW/glfw3.h>
#include <leif/color.h>
#include <leif/ez_api.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
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



void button(const char* text) {
  lf_button(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x222222));
  lf_style_crnt_widget_prop(s.ui, corner_radius, 10);
  lf_widget_set_fixed_height(lf_crnt(s.ui), 12);

  lf_text_h4(s.ui, text);
  lf_style_crnt_widget_prop(s.ui, text_color, LF_WHITE);

  lf_button_end(s.ui);
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x202125));

  lf_ui_core_set_font(s.ui, "/usr/share/fonts/TTF/LilexNerdFont-Regular.ttf");

  for(uint32_t i = 0; i < 1000; i++) {
  lf_div(s.ui);
  lf_widget_set_padding(lf_crnt(s.ui), 20.0f);
  lf_widget_set_margin(lf_crnt(s.ui), 20.0f);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x333333));
  lf_widget_set_corner_radius(s.ui, lf_crnt(s.ui), 10);
  lf_text_p(s.ui, "Hello, World!");
  lf_div_end(s.ui);
  }
 
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
