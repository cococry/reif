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

static int barsize = 30;
static int numdesktops = 9;
static int crntdesktop = 3;


static bool is_big = false;
static void on_click(lf_ui_state_t* ui, lf_widget_t* widget);

void comp(void) {
  lf_div(s.ui);
  lf_crnt(s.ui)->sizing_type = SizingFitToContent;
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);

  lf_div_end(s.ui);
}

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  is_big = !is_big;
  lf_widget_set_padding(widget, is_big ? 20.0f : 5.0f);
}


static void desktop_up(lf_ui_state_t* ui, lf_widget_t* widget) {
  crntdesktop++;
  if(crntdesktop >= numdesktops)
    crntdesktop = 0;
  lf_component_rerender(s.ui, comp);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);
  lf_style_crnt_widget_prop(s.ui, color, LF_WHITE);
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal | AlignCenterVertical); 

  lf_button(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, LF_BLUE);

  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;

  lf_text_h4(s.ui, "Click me");

  lf_button_end(s.ui);



  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
