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

typedef struct {
  lf_ui_state_t* ui;
  lf_widget_t* div;
} state_t;

static state_t s;

static void on_click(lf_ui_state_t* ui, lf_widget_t* widget);

static void comp(void);


static int c = 0;
void comp(void) {
 // printf("Assignment idx before BUTTON_END: %i\n", s.ui->_ez._assignment_idx);
  lf_button(s.ui);
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;
  char buf[64];
  sprintf(buf, "Click %i", c); 
  lf_text_h4(s.ui, buf);
  lf_button_end(s.ui);
  //printf("Assignment idx after BUTTON_END: %i\n", s.ui->_ez._assignment_idx);

  //printf("Childs of root: %i\n", s.ui->root->num_childs);
  lf_text_h4(s.ui, buf);
  printf("Widget ID: %i\n", lf_crnt(s.ui)->id);
}

static void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  c++; // HHAHAHAHAHAHAHA
  lf_component_rerender(ui, comp);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal | AlignCenterVertical); 

  lf_text_h1(s.ui, "Default Text");

  lf_component(s.ui, comp);

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
