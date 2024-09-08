#include "../include/leif/widgets/button.h"


static void _button_render(
  lf_ui_state_t* ui,
  lf_widget_t* div);

void
_button_render(
  lf_ui_state_t* ui,
  lf_widget_t* div) {

}
lf_button_t* 
lf_button(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label) {

  lf_div_t* div = lf_widget_create_from_parent(
    parent, 
    LF_SCALE_CONTAINER(
      parent->container.size.x,
      parent->container.size.y));

  div->props = ui->theme->button_props;
  div->render = _div_render; 
  div->handle_event = NULL; 
}
