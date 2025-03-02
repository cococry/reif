#include "../../include/leif/widgets/div.h"
#include "../../include/leif/util.h"
#include "../../include/leif/layout.h"

static void _div_render(lf_ui_state_t* ui, lf_widget_t* widget);
static void _div_shape(lf_ui_state_t* ui, lf_widget_t* widget);
static void _div_calc_size(lf_ui_state_t* ui, lf_widget_t* widget);

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
  if(!widget) return;
  if(widget->type != WidgetTypeDiv) return;
  lf_widget_apply_layout(ui, widget);
  //printf("  => div size: (%f,%f)\n", widget->container.size.x,
    //  widget->container.size.y);
}

void 
_div_calc_size(lf_ui_state_t* ui, lf_widget_t* widget) {
  if(!widget) return;
  if(widget->type != WidgetTypeDiv) return;
  lf_widget_calc_layout_size(ui, widget);
}

lf_div_t* 
lf_div_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent) {
  if(!parent) return NULL;

  lf_div_t* div = malloc(sizeof(*div));

  div->base = *lf_widget_create(
    ui->crnt_widget_id++,
    WidgetTypeDiv,
    LF_SCALE_CONTAINER(parent->container.size.x,0),
    ui->theme->div_props,
    _div_render,
    NULL,
    _div_shape,
    _div_calc_size
    );

  ui->crnt_widget_id++;

  lf_widget_add_child(parent, (lf_widget_t*)div);

  div->base.layout_type = LayoutVertical;
  div->_column_count = 0;

  return div;
}

void lf_div_set_column_count(
    lf_div_t* div,
    uint32_t column_count) {
  div->_column_count = (int32_t)column_count;
}
