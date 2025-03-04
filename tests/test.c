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

void delay(lf_ui_state_t* ui) {
  usleep((ui->_frame_duration ) * 1000000);
}

bool big = false;

static void comp(void);

void removestuff(lf_ui_state_t* ui, lf_widget_t* widget) {
  big = !big;
  lf_widget_set_padding(s.ui, widget, big ? 40.0f : ui->theme->button_props.padding_right);
  lf_component_rerender(ui, comp);
}


void comp(void) {
  lf_div(s.ui);
  lf_widget_set_fixed_height_percent(lf_crnt(s.ui), 100.0f);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal | AlignCenterVertical);
  lf_div(s.ui);
  lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, lf_color_from_hex(0x555555));
  lf_style_widget_prop(s.ui, lf_crnt(s.ui), corner_radius, 20);

  lf_widget_set_fixed_width(lf_crnt(s.ui), 500.0f);

  for(uint32_t i = 0; i < 5; i++) {
    lf_button(s.ui);
    lf_widget_set_transition_props(lf_crnt(s.ui), 0.1f, lf_ease_out_quad);

    ((lf_button_t*)lf_crnt(s.ui))->on_click = removestuff;

    lf_text_h4(s.ui, "Click Me");

    lf_button_end(s.ui);
  }


  lf_div_end(s.ui);
  lf_text_h1(s.ui, "Text sample");
  lf_div_end(s.ui);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

 
  s.ui->_idle_delay_func = delay;

  lf_widget_set_font_family(s.ui, lf_crnt(s.ui), "CaskaydiaCove Nerd Font");

  lf_component(s.ui, comp);

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
