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

int32_t secs = 0;

void comp(void) {
  lf_div(s.ui);
  char buf[64];
  sprintf(buf, "Seconds elapsed: %i", secs);
  lf_text_h1(s.ui, buf);
  lf_div_end(s.ui);
}

void done(lf_ui_state_t* ui, lf_timer_t* timer) {
  secs++;
  lf_component_rerender(ui, comp);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);
  
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
