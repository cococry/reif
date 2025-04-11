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

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_ui_core_set_window_hint(LF_WINDOWING_HINT_ABOVE, true);
  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  {

  lf_div(s.ui);
    lf_widget_set_layout(lf_crnt(s.ui), LF_LAYOUT_HORIZONTAL);
    lf_widget_set_fixed_height_percent(s.ui, lf_crnt(s.ui), 100.0f); 
    lf_text_h1(s.ui, "Text 1");
    lf_widget_set_pos_x_absolute_percent(lf_crnt(s.ui), 50);
    lf_text_h1(s.ui, "Text 2");
    lf_widget_set_pos_x_absolute_percent(lf_crnt(s.ui), 0);
    lf_text_h1(s.ui, "Text 3");
    lf_widget_set_pos_x_absolute_percent(lf_crnt(s.ui), 100);
  lf_div_end(s.ui);
  }


  lf_widget_invalidate_size_and_layout(s.ui->root);
  lf_widget_shape(s.ui, s.ui->root);
  lf_widget_invalidate_size_and_layout(s.ui->root);
  lf_widget_shape(s.ui, s.ui->root);
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
