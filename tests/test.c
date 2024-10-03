#include <leif/color.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void on_button_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  (void)ui;
  lf_button_t* button = (lf_button_t*)widget;
  printf("Pressed button with label '%s'\n", button->label);
  lf_widget_set_padding(ui, widget, 20);
}

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_win_create(800, 600);
  lf_win_set_title(win, "leif tests");
  lf_win_make_gl_context(win);

  lf_ui_state_t* ui = lf_ui_core_init(win);

  lf_div_t* div = lf_div_create(ui, ui->root);
  div->base.props.color = LF_NO_COLOR;

  lf_button_t* button = lf_button_create_with_label(ui, (lf_widget_t*)div, "Нажмите на меня (Russian)");
  lf_button_create_with_label(ui, (lf_widget_t*)div, "Click Me (English)");

  button->on_click = on_button_click; 
  lf_widget_set_padding(ui, &button->base, 10);
  lf_widget_set_padding(ui, &div->base, 10);

  bool running = true;
  ui->root->props.color = lf_color_from_hex(0x282828);
  div->base.props.color = lf_color_from_hex(0x999999);

  while(running) {
    running = lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
