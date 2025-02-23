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
  lf_widget_set_padding(widget, is_big ? 50.0f : 20.0f);
  lf_widget_set_prop(widget, &widget->props.color.r, is_big ? 255 : 255);
  lf_widget_set_prop(widget, &widget->props.color.g, is_big ? 0 : 255);
  lf_widget_set_prop(widget, &widget->props.color.b, is_big ? 0 : 255);
  lf_widget_set_prop(widget, &widget->props.corner_radius, is_big ? 50 : 0);
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

  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal | AlignCenterVertical); 

  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);

  lf_widget_set_font_size(s.ui, lf_crnt(s.ui), 100);

  for(uint32_t i = 0; i < 2; i++) {
  lf_button(s.ui);
  lf_widget_set_transition_props(lf_crnt(s.ui),
                                 0.3, lf_ease_out_quad);
    lf_widget_set_padding(lf_crnt(s.ui), 20);

    if(i==0)
      ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;

  lf_text_h4(s.ui, "Click me");

  lf_button_end(s.ui);
  }



  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
