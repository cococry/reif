#include <leif/color.h>
#include <leif/layout.h>
#include <leif/leif.h>
#include <leif/widget.h>
#include <leif/widgets/button.h>
#include <leif/widgets/div.h>
#include <leif/win.h>
#include <stdlib.h>
#include <stdbool.h>

#include <runara/runara.h>

float padding = 20;
lf_font_t big_font, small_font;
void on_button_click(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  (void)widget;

  lf_widget_set_padding(widget, widget->props.padding_left + 5); 
  lf_button_t* button = (lf_button_t*)widget;
  lf_button_set_font_size(ui, button, lf_font_get_size(ui, button->font) + 5);
  widget->props.corner_radius = lf_widget_height(widget) / 2.0f;
  lf_ui_core_submit(ui);
}

void on_resize(lf_ui_state_t* ui, lf_widget_t* widget, lf_event_t ev) {
  (void)ui;
  (void)widget;
  if(widget->type != WidgetTypeDiv) return;
  lf_div_t* div = (lf_div_t*)widget;
  lf_div_set_fixed_height(div, ev.height);
  lf_ui_core_submit(ui);
}

int main(void) {
  if(lf_windowing_init() != 0) return EXIT_FAILURE;

  lf_window_t* win = lf_ui_core_create_window(800, 600, "hello leif");
  
  lf_ui_state_t* ui = lf_ui_core_init(win);

  lf_div_t* div = lf_div_create(ui, ui->root);
  lf_widget_set_layout(&div->base, LayoutHorizontal);

  lf_widget_set_listener(&div->base, on_resize, WinEventResize);
  lf_div_set_fixed_height(div, ui->root->container.size.y);

  lf_div_set_flag(div, DivAdjustCenterHorizontal | DivAdjustCenterVertical);

  for(uint32_t i = 0; i < 1; i++) {
    lf_button_t* button = lf_button_create_with_label(ui, &div->base, "Get in touch");
    button->base.props.border_width = 0;
    button->base.props.color = lf_color_from_hex(0x000814);
    button->text_color = LF_WHITE;
    lf_widget_set_padding(&button->base, 20.0f);
    button->base.props.corner_radius = lf_widget_height(&button->base) / 2.0f;
    button->on_click = on_button_click;
  }

  lf_ui_core_submit(ui);

  while(ui->running) {
    lf_ui_core_next_event(ui);
  }
 
  lf_ui_core_terminate(ui);

  return EXIT_SUCCESS;
}
