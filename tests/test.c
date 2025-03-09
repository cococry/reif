#include <GLFW/glfw3.h>
#include <leif/asset_manager.h>
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
#include <unistd.h>
#include <dirent.h>

typedef struct {
  lf_ui_state_t* ui;
  lf_widget_t* div;
} state_t;

static state_t s;

int32_t count = 0;

static void comp(void);

void on_cick(lf_ui_state_t* ui, lf_widget_t* widget) {
  count++;
  lf_component_rerender(ui, comp);
}

void comp(void) {
  lf_div(s.ui);
  lf_text_h4(s.ui, "Button test");
  lf_button(s.ui);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_cick;
  lf_text_h4(s.ui, "Click Me");
  lf_button_end(s.ui);

  lf_div(s.ui);
  lf_widget_set_fixed_height(s.ui, lf_crnt(s.ui), 500);
  lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, lf_color_from_hex(0x282828));
  for(uint32_t i = 0; i < count; i++) {
    lf_text_h4(s.ui, "Element");
  }
  lf_div_end(s.ui);
  
  lf_text_h1(s.ui, "Button test");
  
  lf_div_end(s.ui);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_component(s.ui, comp);
  
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
