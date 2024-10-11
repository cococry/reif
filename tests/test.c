#include <leif/color.h>
#include <leif/layout.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/win.h>
#include <stdlib.h>
#include <stdbool.h>

float padding = 20;
lf_font_t big_font, small_font;
void on_button_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  (void)widget;

  lf_widget_set_padding(widget, widget->props.padding_left + 5); 
  lf_ui_core_submit(ui);
}

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(800, 600, "hello leif");
  lf_ui_state_t* ui = lf_ui_core_init(win);

  lf_div_t* div = lf_div_create(ui, ui->root);
  lf_widget_set_layout(&div->base, LayoutHorizontal);

  lf_div_set_flag(div, DivAdjustCenterHorizontal | DivAdjustCenterVertical);

  for(uint32_t i = 0; i < 2; i++) {
    lf_button_t* button = lf_button_create_with_label(ui, &div->base, "hello world");
    button->base.props.border_width = 1;
    button->base.props.border_color = LF_BLACK;
    button->base.props.corner_radius = 0;
  }

  lf_ui_core_submit(ui);

  while(ui->running) {
    lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
