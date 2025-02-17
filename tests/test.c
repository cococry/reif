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
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  lf_ui_state_t* ui;
  lf_widget_t* div;
} state_t;

static state_t s;


static void maincomp(void);

static lf_widget_t* rerender = NULL;

int counter = 0;
bool active = false;
void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  counter++;
  active = !active;
  lf_ez_api_set_assignment_only_mode(s.ui, true);
  s.ui->_ez._assignment_idx = 0;
  maincomp();
  lf_ez_api_set_assignment_only_mode(s.ui, false);
  lf_ui_core_rerender_widget(ui, rerender); 
}

void maincomp(void) {
  rerender = &lf_div(s.ui)->base;

  char buf[64];
  sprintf(buf, "Counter: %i", counter);
  lf_text_h1(s.ui, buf);

  lf_button(s.ui);
  if(active) {
    lf_style_crnt_widget_prop(s.ui, color, LF_RED);
  } else {
    lf_style_crnt_widget_prop(s.ui, color, s.ui->theme->button_props.color);
  }
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;

  lf_text_h4(s.ui, "Increase");


  lf_button_end(s.ui);

  lf_div_end(s.ui);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);
  maincomp();
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

 
  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
