#include <GLFW/glfw3.h>
#include <leif/color.h>
#include <leif/ez_api.h>
#include <leif/layout.h>
#include <leif/event.h>
#include <leif/leif.h>
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
  lf_text_t* time_text;
  lf_ui_state_t* ui;
  lf_timer_t* tick_timer;
} state_t;

static state_t s;

static lf_font_t heading_chars;

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  (void)ui;
  printf("Clicked button!\n");
}



void button(const char* text) {
  lf_button(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x222222));
  lf_style_crnt_widget_prop(s.ui, corner_radius, 10);
  lf_widget_set_fixed_height(lf_crnt(s.ui), 12);

  lf_text_h4(s.ui, text);
  lf_style_crnt_widget_prop(s.ui, text_color, LF_WHITE);

  lf_button_end(s.ui);
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x202125));

  lf_ui_core_set_font(s.ui, "/usr/share/fonts/TTF/LilexNerdFont-Regular.ttf");

  lf_div(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x111111));
  lf_style_crnt_widget_prop(s.ui, corner_radius, 10);
  lf_widget_set_padding(lf_crnt(s.ui), 20);
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_widget_set_margin(lf_crnt(s.ui), 20);
  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterVertical);

  button("File");
  button("Edit");
  button("Open");

  lf_div_end(s.ui);

  lf_div(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x111111));
  lf_widget_set_layout(lf_crnt(s.ui), LayoutHorizontal);
  lf_crnt(s.ui)->sizing_type = SizingGrow;
  lf_widget_set_margin(lf_crnt(s.ui), 20);
  lf_style_crnt_widget_prop(s.ui, margin_top, 0);
  lf_widget_set_padding(lf_crnt(s.ui), 10);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 10);

  lf_div(s.ui);
  lf_crnt(s.ui)->sizing_type = SizingGrow;
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x333333));
  lf_widget_set_margin(lf_crnt(s.ui), 20);
  lf_widget_set_padding(lf_crnt(s.ui), 10);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 10);
  lf_widget_set_fixed_width(lf_crnt(s.ui), 350);
 

  lf_div(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x555555));
  lf_style_crnt_widget_prop(s.ui, corner_radius, 5);
  lf_text_h4(s.ui, " Industrial Society");
  lf_div_end(s.ui);
  lf_div(s.ui);
  lf_text_h4(s.ui, " Comunist Manifesto");
  lf_div_end(s.ui);
  lf_div(s.ui);
  lf_text_h4(s.ui, " Gott ist tot");
  lf_div_end(s.ui);
  lf_div(s.ui);
  lf_text_h4(s.ui, " Friedrich Nietzsche");
  lf_div_end(s.ui);

  lf_div_end(s.ui);

  lf_div(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x333333));
  lf_crnt(s.ui)->sizing_type = SizingGrow;
  lf_widget_set_margin(lf_crnt(s.ui), 20);
  lf_widget_set_padding(lf_crnt(s.ui), 10);
  lf_style_crnt_widget_prop(s.ui, corner_radius, 10);

  lf_text_h2(s.ui, "INDUSTRIAL SOCIETY AND ITS FUTURE");
  lf_style_crnt_widget_prop(s.ui, margin_bottom, 15);

  lf_text_h4(s.ui, "The Industrial Revolution and its consequences have been a disaster for the human race. They have greatly increased the life-expectancy of those of us who live in “advanced” countries, but they have destabilized society, have made life unfulfilling, have subjected human beings to indignities, have led to widespread psychological suffering (in the Third World to physical suffering as well) and have inflicted severe damage on the natural world. The continued development of technology will worsen the situation. It will certainly subject human beings to greater indignities and inflict greater damage on the natural world, it will probably lead to greater social disruption and psychological suffering, and it may lead to increased physical suffering even in “advanced” countries.");

  lf_div_end(s.ui);

  lf_div_end(s.ui);
 
  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
