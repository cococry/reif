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


void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(widget->props.padding_top == ui->theme->button_props.padding_top) {
    lf_widget_set_padding(widget, 20);
  } else {
    lf_widget_set_padding(widget, ui->theme->button_props.padding_top); 
  }
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_div(s.ui);

  lf_widget_set_alignment(lf_crnt(s.ui), AlignCenterHorizontal);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x444444));

  lf_widget_set_max_width(lf_crnt(s.ui), 500.0f);
  lf_widget_set_min_height(lf_crnt(s.ui), 600.0f);

  lf_text_h4(s.ui, "The Industrial Revolution and its consequences have been a disaster for the human race. They have greatly increased the life-expectancy of those of us who live in “advanced” countries, but they have destabilized society, have made life unfulfilling, have subjected human beings to indignities, have led to widespread psychological suffering (in the Third World to physical suffering as well) and have inflicted severe damage on the natural world. The continued development of technology will worsen the situation.");
  lf_style_crnt_widget_prop(s.ui, text_align, ParagraphAlignmentCenter);

  for(uint32_t i = 0; i < 10; i++) {
    lf_button(s.ui);
    ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;
    lf_widget_set_font_style(s.ui, lf_crnt(s.ui), LF_FONT_STYLE_BOLD);

    lf_text_h4(s.ui, "Learn More");

    lf_button_end(s.ui);
  }
  lf_div_end(s.ui);


  lf_text_h1(s.ui, "Nostalgia");
  lf_style_crnt_widget_prop(s.ui, margin_top, 10);


  lf_widget_shape(s.ui, s.ui->root);

  while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
