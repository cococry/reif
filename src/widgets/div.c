#include "../include/leif/widgets/div.h"
#include "../include/leif/util.h"
#include "../include/leif/layout.h"

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
  lf_widget_apply_layout(widget);
}

lf_div_t* 
lf_div_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent) {

  lf_div_t* div = malloc(sizeof(*div));

  div->base = *lf_widget_create(
    WidgetTypeDiv,
    LF_SCALE_CONTAINER(parent->container.size.x,0),
    ui->theme->div_props,
    _div_render,
    NULL,
    _div_shape);

  lf_widget_add_child(parent, (lf_widget_t*)div);

  div->base.layout_type = LayoutVertical;

  return div;
}

void 
lf_div_destroy(lf_div_t* div) {
  if(!div) return;

  lf_widget_destroy(&div->base);

  free(div);
}
