#include <leif/color.h>
#include <leif/layout.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

float padding = 20;
lf_font_t big_font, small_font;
void on_button_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  (void)widget;
  lf_button_t* button = (lf_button_t*)widget;
  lf_button_set_font(ui, button, button->font == small_font ? big_font : small_font);
}

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_win_create(800, 600);
  lf_win_set_title(win, "leif tests");
  lf_win_make_gl_context(win);

  lf_ui_state_t* ui = lf_ui_core_init(win);

  big_font = lf_load_font_from_name(ui, "Inter", 40); 
  small_font = lf_load_font_from_name(ui, "Inter", 16); 

  lf_div_t* div = lf_div_create(ui, ui->root);

  lf_button_t* my_button = lf_button_create_with_label_ex(ui, &div->base, "Hello Papa", 
                                                          small_font);
  my_button->on_click = on_button_click;

  lf_widget_shape(ui, ui->root);
  printf("Root height: %f\n", ui->root->container.size.y);


  bool running = true;

  while(running) {
    running = lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
