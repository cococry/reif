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
  lf_window_t win2 = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);
  lf_ui_state_t* ui2 = lf_ui_core_init(win2);

  s.ui->root->props.color = LF_NO_COLOR;
  ui2->root->props.color = LF_NO_COLOR;

  lf_text_h1(ui2, "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6q7r8s9t0u1v2w3x4y5z6");
  lf_text_h1(s.ui, "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6q7r8s9t0u1v2w3x4y5z6");
lf_text_h2(s.ui, "1234567890abcde123fghijkLMNOPQRSTuvwxzy6789");
lf_text_h3(s.ui, "asQwErTyU12345zZ7gFdX");
lf_text_h4(s.ui, "hGf456jklP@#$%&7LM");
lf_text_h5(s.ui, "abcd12345fGh6jkl987XY");
lf_text_p(s.ui, "abcdeFGHIJ9876543klmnOpqrSTUVx@#");
lf_text_p(s.ui, "abcdeFGHIJ9876543klmnOpqrSTUVx@#");
lf_text_p(s.ui, "abcdeFGHIJ9876543klmnOpqrSTUVx@#");
lf_text_p(s.ui, "abcdeFGHIJ9876543klmnOpqrSTUVx@#");
lf_text_p(s.ui, "abcdeFGHIJ9876543klmnOpqrSTUVx@#");

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
    lf_ui_core_next_event(ui2);
  }
  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
