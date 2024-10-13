#include "../include/leif/widgets/div.h"
#include "../include/leif/util.h"
#include "../include/leif/layout.h"

static void _div_render(lf_ui_state_t* ui, lf_widget_t* widget);
static void _div_shape(lf_ui_state_t* ui, lf_widget_t* widget);

void 
_div_render(lf_ui_state_t* ui, lf_widget_t* widget) {
  lf_div_t* div = (lf_div_t*)widget;
  ui->render_rect(
    ui->render_state, div->base.container.pos,
    LF_WIDGET_SIZE_V2(widget),
    widget->props.color, widget->props.border_color, 
    widget->props.border_width, widget->props.corner_radius);

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
  div->fixed_width = -1.0f;
  div->fixed_height = -1.0f;
  div->flags = 0;

  return div;
}

void 
lf_div_set_fixed_width(
    lf_div_t* div,
    float width) {
  if(div->fixed_width == width) return;

  div->fixed_width = width;
  div->base.container.size.x = width;
}

void 
lf_div_set_fixed_height(
    lf_div_t* div,
    float height) {
  height -= div->base.props.padding_top + div->base.props.padding_bottom;
  if(div->fixed_height == height) return;

  div->fixed_height = height;
  div->base.container.size.y = height;
}

void lf_div_set_flag(
    lf_div_t* div, 
    lf_div_flag_t flag
    ) {

  div->flags |= flag;
}

void lf_div_unset_flag(
    lf_div_t* div, 
    lf_div_flag_t flag
    ) {
  div->flags &= ~flag;
}

bool lf_div_has_flag(
    lf_div_t* div,
    lf_div_flag_t flag
    ) {
  return (div->flags & flag) != 0;
}
