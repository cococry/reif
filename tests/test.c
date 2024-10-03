#include <leif/color.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

lf_font_t big_font, small_font;
void on_button_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)widget;
  (void)ui;
  lf_button_t* button = (lf_button_t*)widget;
  if(button->font == small_font)
    lf_button_set_font(ui, button, big_font);
  else
    lf_button_set_font(ui, button, small_font);
}

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_win_create(800, 600);
  lf_win_set_title(win, "leif tests");
  lf_win_make_gl_context(win);

  lf_ui_state_t* ui = lf_ui_core_init(win);

  lf_div_t* div = lf_div_create(ui, ui->root);
  div->base.props.color = LF_NO_COLOR;

  lf_button_t* button = lf_button_create_with_label(ui, (lf_widget_t*)div, "Button");

  button->on_click = on_button_click; 
  button->base.props.padding_left =  20;
  button->base.props.padding_right=  20;
  lf_widget_set_padding(ui, &div->base, 10);
  
  big_font = lf_load_font_from_name(ui, "Inter", 40); 
  small_font = lf_load_font_from_name(ui, "Inter", 24); 

  lf_button_set_font(ui, button, small_font);
  bool running = true;

  while(running) {
    running = lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
