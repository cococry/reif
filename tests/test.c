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


  uint32_t barcolor = 0x333333;

uint32_t n = 1;
static void comp(void);

void on_timer(lf_ui_state_t* ui, lf_timer_t* widget) {
  n++;
  lf_component_rerender(ui, comp);
}

void comp(void) {
  lf_div(s.ui);
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_sizing(lf_crnt(s.ui), SizingFitToContent);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical);
  lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, lf_color_dim(lf_color_from_hex(barcolor), 0.8));

  for(uint32_t i = 0; i < n; i++) {
    lf_button_t* btn = lf_button(s.ui);
    lf_widget_set_transition_props(&btn->base, 0.2f, lf_ease_out_quad);
    if(!btn->_held)
      lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color,
                                 (i == 3 ? LF_WHITE : lf_color_from_hex(0x999999)));
    //lf_widget_set_padding(s.ui, lf_crnt(s.ui), 0);
    lf_widget_set_fixed_width(s.ui, lf_crnt(s.ui), 10);
    lf_widget_set_fixed_height(s.ui, lf_crnt(s.ui), 10);
    lf_style_widget_prop(s.ui, lf_crnt(s.ui), corner_radius, 10 / 2.0f);
    lf_button_end(s.ui);
  }
  lf_div_end(s.ui);
}


void uidesktops(void) {
  lf_div(s.ui);
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_sizing(lf_crnt(s.ui), SizingFitToContent);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical);

  for(uint32_t i = 0; i < 9; i++) {
    lf_button(s.ui);
    lf_widget_set_transition_props(lf_crnt(s.ui), 0.2f, lf_ease_out_quad);
    lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color,
                               (i == 3 ? LF_WHITE : lf_color_from_hex(0x999999)));
    lf_widget_set_padding(s.ui, lf_crnt(s.ui), 0);
    lf_widget_set_fixed_width(s.ui, lf_crnt(s.ui), 10);
    lf_widget_set_fixed_height(s.ui, lf_crnt(s.ui), 10);
    lf_style_widget_prop(s.ui, lf_crnt(s.ui), corner_radius, 3);
    lf_button_end(s.ui);
  }
  lf_div_end(s.ui);
}


int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
    s.ui = lf_ui_core_init(win);


  s.ui = lf_ui_core_init(win);
  s.ui->root->props.color = LF_NO_COLOR;

  lf_widget_set_font_family(s.ui, s.ui->root, "JetBrainsMono Nerd Font");
  lf_widget_set_font_style(s.ui, s.ui->root, LF_FONT_STYLE_BOLD);

 
  lf_button(s.ui);
 
  lf_text_h4(s.ui,   "Window");
  lf_button_end(s.ui);
  lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), text_color, LF_BLACK);
  lf_widget_set_padding(s.ui, lf_crnt(s.ui), 20);
   
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
