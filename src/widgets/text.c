#include "../../include/leif/widgets/text.h"
#include <wctype.h>

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

void 
_recalculate_label(
  lf_ui_state_t* ui,
  lf_text_t* text
) {
  lf_widget_t* widget = &text->base;
  lf_text_dimension_t text_dimension = ui->render_get_paragraph_dimension(
    ui->render_state,
    text->label,
    text->font,
    (lf_paragraph_props_t){
      .wrap = widget->parent->container.pos.x + widget->parent->container.size.x + 10, 
      .align = ParagraphAlignmentLeft
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
  ui->render_rect(
    ui->render_state, 
    widget->container.pos,
   LF_WIDGET_SIZE_V2(widget), 
    widget->props.color, widget->props.border_color,
    widget->props.border_width, widget->props.corner_radius);

  if(text->label) {
    vec2s text_pos =(vec2s){
      .x = widget->container.pos.x + widget->props.padding_left, 
      .y =  widget->container.pos.y + widget->props.padding_top 
    };

    ui->render_paragraph(
      ui->render_state,
      text->label,
      text->font,
      text_pos,
      widget->props.text_color,
      (lf_paragraph_props_t){
        .wrap = widget->parent->container.pos.x + widget->parent->container.size.x + widget->parent->props.padding_left, 
        .align = ParagraphAlignmentLeft
      }
    );
  }
}

lf_text_t* _text_create(
  lf_ui_state_t* ui,
  lf_widget_t* parent,
  const char* label,
  lf_font_t font) {
  if(!parent) return NULL;
  lf_text_t* text = (lf_text_t*)malloc(sizeof(lf_text_t));

  text->label = (char*)label;
  text->font = font;
  lf_text_dimension_t text_dimension = ui->render_get_paragraph_dimension(
    ui->render_state,
    text->label,
    text->font,
    (lf_paragraph_props_t){
        .wrap = parent->container.pos.x + parent->container.size.x + parent->props.padding_left,
      .align = ParagraphAlignmentLeft
    }
  );

  text->_text_dimension = text_dimension;
  text->base = *lf_widget_create(
    WidgetTypeText,
    LF_SCALE_CONTAINER(text_dimension.width, text_dimension.height),
    ui->theme->text_props,
    _text_render, 
    NULL,
   _text_shape
  );

  text->base.layout_type = LayoutNone;
  lf_widget_add_child(parent, (lf_widget_t*)text);

  return text;
}

lf_text_t* lf_text_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label) {
   return _text_create(ui, parent, label, ui->fonts[TextLevelParagraph]);
}

lf_text_t* lf_text_create_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label,
    lf_font_t font) {
   return _text_create(ui, parent, label, font);
}

void lf_text_set_font(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    void* font) {
  text->font = font;
  _recalculate_label(ui, text);
}

void 
lf_text_set_label(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    const char* label) {
  if(text->_changed_label) {
    free(text->label);
  }
  text->_changed_label = false;
  text->label = strdup(label);
  _recalculate_label(ui, text);
}

void lf_text_set_font_size(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    uint32_t size) {
#ifdef LF_RUNARA
  if(!text->_changed_font_size) {
    text->font = lf_load_font(ui, ((RnFont*)text->font)->filepath, size);
  } else {
    lf_font_resize(ui, text->font, size);
  }
  lf_text_set_font(ui, text, text->font);
#endif
}

