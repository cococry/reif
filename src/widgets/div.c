#include "../include/leif/widgets/button.h"

static void _div_render(lf_ui_state_t* ui, lf_widget_t* div);

void 
_div_render(lf_ui_state_t* ui, lf_widget_t* div) {
  lf_ui_core_begin_render(
    ui, 
    div->container.size.x,
    div->container.size.y);

  ui->render_rect(
    ui->render_state, div->container.pos,
    div->container.size, 
    div->props.color, LF_NO_COLOR,
    0.0f, 0.0f);

  lf_ui_core_end_render(ui);
}

lf_div_t* 
lf_div(
  lf_ui_state_t* ui,
  lf_widget_t* parent) {
  lf_div_t* div = lf_widget_create_from_parent(
    parent, 
    LF_SCALE_CONTAINER(
      parent->container.size.x,
      parent->container.size.y));

  div->props = ui->theme->div_props;
  div->render = _div_render; 
  div->handle_event = NULL; 

  return div;
}

void lf_div_set_layout(
    lf_div_t* div, 
    lf_div_layout_type_t layout) {

  if(!div) return;
  div->layout = layout;
  lf_div_calculate_childs(div);
}

void 
lf_div_calculate_childs(lf_div_t* div) {
  if(!div) return;
  lf_widget_t* widget = div->base;
  if(!widget) return;

  if(div->layout == LayoutVertical) {
    float y_before = widget->container.pos.y;
    float y_ptr = y_before;
    for(uint32_t i = 0; i < widget->num_childs; i++) {
      lf_widget_t* child = widget->childs[i];

      child->container.pos.x = widget->container.pos.x;
      child->container.pos.y = y_ptr;

      y_ptr += lf_widget_height(child); 
    }

    widget->container.size.y = y_ptr - y_before;
  }
}
