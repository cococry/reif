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
  lf_text_t* text;
} state_t;

static state_t s;

void on_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(s.text->font.style.style != LF_FONT_STYLE_ITALIC)
    lf_widget_set_font_style(ui, &s.text->base, LF_FONT_STYLE_ITALIC);
  else 
    lf_widget_set_font_style(ui, &s.text->base, LF_FONT_STYLE_REGULAR);

  lf_ui_core_submit(ui);
}

int main(void) {

  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t win = lf_ui_core_create_window(1280, 720, "hello leif");
  s.ui = lf_ui_core_init(win);

  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0xFFFFFF));


  lf_div(s.ui);
  lf_widget_set_max_width(lf_crnt(s.ui), 500.0f);

  s.text = lf_text_h1(s.ui, "Hello");
  lf_style_crnt_widget_prop(s.ui, text_color, LF_BLACK);

  lf_button(s.ui);
  lf_style_crnt_widget_prop(s.ui, color, lf_color_from_hex(0x01017a));
  ((lf_button_t*)lf_crnt(s.ui))->on_click = on_click;

  lf_text_h1(s.ui, "Italic");
  lf_style_crnt_widget_prop(s.ui, text_color, LF_WHITE);

  lf_button_end(s.ui);

  lf_div_end(s.ui);
    while(s.ui->running) {
    lf_ui_core_next_event(s.ui);
  }
 
  lf_ui_core_terminate(s.ui);

  return EXIT_SUCCESS;
}
