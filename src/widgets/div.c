#include "../include/leif/widgets/div.h"
#include "../include/leif/util.h"

static void _div_render(lf_ui_state_t* ui, lf_widget_t* widget);

void 
_div_render(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_div_t* div = (lf_div_t*)widget;

  ui->render_rect(
    ui->render_state, div->base.container.pos,
    LF_WIDGET_SIZE_V2(widget),
    widget->props.color, LF_NO_COLOR,
    0.0f, 0.0f);
}

void 
_div_shape(lf_ui_state_t* ui, lf_widget_t* widget) {
  (void)ui;
  if(!widget) return;
  if(widget->type != WidgetTypeDiv) return;

  widget->container = widget->parent->container;

  lf_div_t* div = (lf_div_t*)widget;

  if(div->layout == LayoutVetical) {
    float y_before = widget->container.pos.y + widget->props.padding_top;
    float y_ptr = y_before;
    for(uint32_t i = 0; i < widget->num_childs; i++) {
      lf_widget_t* child = widget->childs[i];

      child->container.pos.x = widget->container.pos.x + widget->props.padding_left;
      child->container.pos.y = y_ptr;

      y_ptr += lf_widget_height(child);
    }

    widget->container.size.y = y_ptr - y_before;
  }

}

lf_div_t* 
lf_div_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent) {

  lf_div_t* div = malloc(sizeof(*div));

  div->base = *lf_widget_create(
    WidgetTypeDiv,
    LF_SCALE_CONTAINER(
      parent->container.size.x,
      parent->container.size.y), 
    ui->theme->div_props,
    _div_render,
    NULL,
    _div_shape);

  lf_widget_add_child(parent, (lf_widget_t*)div);

  div->layout = LayoutVetical;

  return div;
}

void 
lf_div_destroy(lf_div_t* div) {
  if(!div) return;

  lf_widget_destroy(&div->base);

  free(div);
}

void lf_div_set_layout(
    lf_div_t* div, 
    lf_div_layout_type_t layout) {

  if(!div) return;
  div->layout = layout;
}

