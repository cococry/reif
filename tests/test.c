#include <leif/leif.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_win_create(800, 600);
  lf_win_set_title(win, "leif tests");

  lf_win_make_gl_context(win);
  lf_ui_state_t* ui = lf_ui_core_init(win);

  ui->root->props.color = (lf_color_t){50, 50, 50, 255};

  bool running = true;
  while(running) {
    running = lf_ui_core_next_event(ui);
  }
  

  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
