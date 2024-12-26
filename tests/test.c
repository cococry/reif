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
#include <unistd.h>

typedef struct {
  lf_text_t* time_text;
  lf_ui_state_t* ui;
  lf_timer_t* tick_timer;
} state_t;

static state_t s;

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  (void)ui;
  printf("Clicked button!\n");
}


int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_ui_core_set_window_flags(LF_WINDOWING_X11_OVERRIDE_REDIRECT);
  lf_window_t* win = lf_ui_core_create_window(500, 500, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_div(s.ui);
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_style_crnt_widget_prop(s.ui, color, LF_RED);
  lf_widget_set_margin(lf_crnt(s.ui), 20);

  lf_text_h1(s.ui, "Hello, World!");

  lf_button(s.ui);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click; 

  lf_text_h1(s.ui, "Click me");
  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);

  lf_button_end(s.ui);

  lf_div_end(s.ui);
 
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
