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

static int count = 0;
bool active = true;

static void comp(void);

static int size = 200.0f;
static int clicks = 0;

void addsize(lf_ui_state_t* ui, lf_widget_t* widget) {
  size += 20.0f;
  clicks++;
  lf_component_rerender(ui, comp);
}

void removesize(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(size - 20.0f > 200.0f)
    size -= 20.0f;
  else 
    size = 200.0f;

  if(clicks - 1 >= 0)
    clicks--;
  lf_component_rerender(ui, comp);
}

void reset(lf_ui_state_t* ui, lf_widget_t* widget) {
  size = 200.0f; 
  clicks = 0;
  lf_component_rerender(ui, comp);
}

void on_hover(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(ui, widget, corner_radius, 35);
}

void on_leave(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_style_widget_prop(ui, widget, corner_radius, 20);
}


void comp(void) {
  lf_div(s.ui);

  for(uint32_t i = 0; i < 3; i++) {
  lf_div(s.ui);
  lf_crnt(s.ui)->scroll_offset.y = 0; 
  lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, lf_color_from_hex(0x33333));
  lf_widget_set_fixed_width(s.ui, lf_crnt(s.ui), 500.0f);
  lf_widget_set_fixed_height(s.ui, lf_crnt(s.ui), 500.0f);
  lf_widget_set_padding(s.ui, lf_crnt(s.ui), 40);
  lf_widget_set_margin(s.ui, lf_crnt(s.ui), 40);
  lf_widget_set_transition_props(lf_crnt(s.ui), 0.2f, lf_ease_out_quad);


  for(uint32_t i = 0; i < 100; i++) {
    lf_div(s.ui);
    char buf[64];
    sprintf(buf, "Hello %i", i + 1);
    lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, lf_color_from_hex(i % 2 == 0 ? 0x333333 : 0x222222));
    lf_text_h1(s.ui, buf); 
    lf_div_end(s.ui);
  }


  lf_div_end(s.ui);
  }
  lf_div_end(s.ui);
}
int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

 
  s.ui->_idle_delay_func = delay;

  lf_div(s.ui);
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_fixed_height_percent(s.ui, lf_crnt(s.ui), 100.0f);

  for(uint32_t i = 0; i < 50; i++) {
    lf_div(s.ui);
    lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), color, lf_color_from_hex(0x33333));
    lf_style_widget_prop(s.ui, lf_crnt(s.ui), margin_bottom, 10); 
    lf_widget_set_sizing(lf_crnt(s.ui), SizingFitToContent);
    lf_widget_set_fixed_height(s.ui, lf_crnt(s.ui), 200);
    
    {
      for(uint32_t i = 0; i < 4; i++) {
        lf_button(s.ui);
        lf_style_widget_prop(s.ui, lf_crnt(s.ui), corner_radius, i % 2 == 0 ? 20 : 0);
        lf_style_widget_prop(s.ui, lf_crnt(s.ui), border_width, i % 2 != 0 ? 3 : 0);
        lf_style_widget_prop_color(s.ui, lf_crnt(s.ui), border_color, LF_RED); 
        lf_widget_set_transition_props(lf_crnt(s.ui), 0.2f, lf_ease_out_quad);
        {
          lf_text_h1(s.ui, "Click Me!");
        }
        lf_button_end(s.ui);
      }
    }
    lf_div_end(s.ui);
  }
  lf_div_end(s.ui);

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }

  lf_ui_core_terminate(s.ui);
	
  return EXIT_SUCCESS;
}
