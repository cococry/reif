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
void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_ui_core_start_timer(ui, 1.0f, print);
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_ui_core_set_window_hint(LF_WINDOWING_HINT_ABOVE, true);
  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  mydiv = lf_div(s.ui); 
  lf_widget_set_padding(s.ui, lf_crnt(s.ui), 0);
  lf_widget_set_margin(s.ui, lf_crnt(s.ui), 0);
  lf_widget_set_layout(lf_crnt(s.ui), LF_LAYOUT_HORIZONTAL);
  lf_widget_set_alignment(lf_crnt(s.ui), LF_ALIGN_CENTER_HORIZONTAL | LF_ALIGN_CENTER_VERTICAL);

  lf_button(s.ui);

  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;
  lf_text_h4(s.ui, "Click"); 
  lf_button_end(s.ui);

  lf_div_end(s.ui);

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
