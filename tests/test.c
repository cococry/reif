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

static int barsize = 30;
static int numdesktops = 9;
static int crntdesktop = 3;


static bool is_big = false;
static void on_click(lf_ui_state_t* ui, lf_widget_t* widget);

static void comp(void);

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(crntdesktop + 1 > numdesktops)
    crntdesktop = 0;
  else 
    crntdesktop++;
  lf_component_rerender(s.ui, comp);
}

static bool showing_dialogue = false;

static int nelements = 1;
void onclick(lf_ui_state_t* ui, lf_widget_t* widget) {
  nelements++;
  lf_component_rerender(s.ui, comp);
}
void onclickdec(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(nelements - 1 >= 0)
    nelements--;
  lf_component_rerender(s.ui, comp);
}

void comp(void) {
  lf_div(s.ui);
  lf_div(s.ui);
  lf_text_h4(s.ui, "Click me");

  char buf[64];
  sprintf(buf, "Elements: %i", nelements);
  lf_text_h1(s.ui, buf); 
  lf_div_end(s.ui);
  lf_div_end(s.ui);
  lf_text_h4(s.ui, "Hello");
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


  lf_div(s.ui);
  lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, LF_RED);
  lf_text_h1(s.ui, "Hello, World!");
  lf_div_end(s.ui);
  lf_text_h4(s.ui, "world");

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
