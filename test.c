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
#include <wctype.h>
#include <stdio.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
  lf_ui_state_t* ui;
  lf_widget_t* div;
} state_t;

static state_t s;

float val = 0;

lf_div_t* mydiv;
void timer(lf_ui_state_t* ui, lf_timer_t* timer) {
  lf_widget_set_fixed_height(s.ui, &mydiv->base, 0.0f);
}

void print(lf_ui_state_t* ui, lf_timer_t* timer) {
  (void)ui; 
  (void)timer;
  printf("Finsihed.\n");
}
int count = 0;
char* text = "Button Text";

int32_t percents[] = {
  20, 
  82
};

void comp(lf_ui_state_t* ui) {
  for(uint32_t i = 0; i < 2; i++) {
    char buf[32];
    sprintf(buf, "%i%%", percents[i]);
    lf_text_h4(ui, buf);
  }
}
void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  percents[1] = 81;
  lf_component_rerender(s.ui, comp);
}


int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_ui_core_set_window_hint(LF_WINDOWING_HINT_ABOVE, true);
  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_div(s.ui);
  lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, lf_color_from_hex(0x444444));
  lf_style_widget_prop(s.ui, lf_crnt(s.ui), corner_radius_percent, 5); 
  lf_widget_set_fixed_height(s.ui, lf_crnt(s.ui), 500.0f);
  lf_widget_set_fixed_width(s.ui, lf_crnt(s.ui), 500.0f);

  lf_div(s.ui);
  lf_widget_set_layout(lf_crnt(s.ui), LF_LAYOUT_HORIZONTAL);

  lf_component(s.ui, comp);
  
  lf_button(s.ui)->on_click = on_click;
  lf_text_h4(s.ui, "Button");
  lf_button_end(s.ui);

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
