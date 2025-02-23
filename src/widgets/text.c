#include "../../include/leif/widgets/text.h"
#include "../../include/leif/ez_api.h"
#include <cglm/types-struct.h>

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

void 
_recalculate_label(
  lf_ui_state_t* ui,
  lf_text_t* text
) {
  lf_widget_t* widget = &text->base;
  vec2s text_pos = (vec2s){
    .x = widget->container.pos.x + widget->props.padding_left, 
    .y =  widget->container.pos.y + widget->props.padding_top 
  };

  float wrap = widget->parent->container.pos.x + widget->parent->container.size.x - widget->parent->props.padding_right;
  if(widget->parent->sizing_type == SizingFitToContent) {
    wrap = -1.0f;
  }
  lf_text_dimension_t text_dimension = ui->render_get_paragraph_dimension(
    ui->render_state,
    text->label,
    text_pos,
    text->font.font,
    (lf_paragraph_props_t){
      .wrap = wrap, 
      .align = widget->props.text_align
    }
  );
  text->base.container.size.x = text_dimension.width; 
  text->base.container.size.y = text_dimension.height;
  text->_text_dimension = text_dimension;
}

void
_text_shape(
  lf_ui_state_t* ui, 
  lf_widget_t* widget
) {
  if(widget->type != WidgetTypeText || !widget) return;
  lf_text_t* text = (lf_text_t*)widget;
  _recalculate_label(ui, text);
}

void
_text_render(
  lf_ui_state_t* ui,
  lf_widget_t* widget) { 
  if(!widget) return;

  lf_text_t* text = (lf_text_t*)widget;
  vec2s text_pos = (vec2s){
    .x = widget->container.pos.x + widget->props.padding_left, 
    .y =  widget->container.pos.y + widget->props.padding_top 
  };

  float wrap = widget->parent->container.pos.x + 
   widget->parent->container.size.x - widget->parent->props.padding_right;
   
  if(widget->parent->sizing_type == SizingFitToContent) {
    wrap = -1.0f;
  }

  ui->render_rect(
    ui->render_state, 
    widget->container.pos,
    LF_WIDGET_SIZE_V2(&text->base), 
    widget->props.color, widget->props.border_color,
    widget->props.border_width, widget->props.corner_radius);
  if(text->label) {
    ui->render_paragraph(
      ui->render_state,
      text->label,
      text->font.font,
      text_pos,
      widget->props.text_color,
      (lf_paragraph_props_t){
        .wrap = wrap, 
        .align = widget->props.text_align
      }
    );
  }
}

lf_text_t* _text_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent,
  const char* label,
  lf_mapped_font_t font) {
  if(!parent) return NULL;
  lf_text_t* text = (lf_text_t*)malloc(sizeof(lf_text_t));

  text->label = strdup(label);
  text->font = font;

  lf_text_dimension_t text_dimension = ui->render_get_paragraph_dimension(
    ui->render_state,
    text->label,
    parent->container.pos,
    text->font.font,
    (lf_paragraph_props_t){
      .wrap = parent->container.pos.x + parent->container.size.x, 
      .align = 1
    }
  );

  text->_text_dimension = text_dimension;

  lf_widget_props_t props = ui->theme->text_props;

  text->base = *lf_widget_create(
    ui->crnt_widget_id++,
    WidgetTypeText,
    LF_SCALE_CONTAINER(text_dimension.width, text_dimension.height),
    props,
    _text_render, 
    NULL,
    NULL,
    _text_shape
  );

  text->base.props.text_color = parent->props.text_color;
  text->base.layout_type = LayoutNone;
  lf_widget_add_child(parent, (lf_widget_t*)text);

  _recalculate_label(ui, text);

  return text;
}


lf_text_t* lf_text_create_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label,
    lf_mapped_font_t font) {
   return _text_create(ui, parent, label, font);
}


void lf_text_set_font(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    const char* family_name,
    lf_font_style_t style,
    uint32_t pixel_size) {

  text->font = lf_asset_manager_request_font(ui, family_name, style, pixel_size);
  _recalculate_label(ui, text);
}

void 
lf_text_set_label(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    const char* label) {
  if(strcmp(text->label, label) == 0) return;
  if(text->_changed_label) {
    free(text->label);
  }
  text->_changed_label = false;
  text->label = strdup(label);
  _recalculate_label(ui, text);
}
