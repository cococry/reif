#include "../../include/leif/widgets/text.h"
#include <cglm/types-struct.h>

#ifdef LF_RUNARA
#include <runara/runara.h>
#endif

int32_t getchildidxof(lf_widget_t* child, lf_widget_t* parent) {
  for(uint32_t i = 0; i < parent->num_childs; i++) {
    if(parent->childs[i] == child) return i;
  }
  return -1; 
}
void 
_recalculate_label(
  lf_ui_state_t* ui,
  lf_text_t* text
) {
  if(!text->base._needs_size_calc) return;
  lf_widget_t* widget = &text->base;
  if(widget->parent->sizing_type == LF_SIZING_FIT_CONTENT && !widget->_changed_size) {
    return;
  }

  float rightsiblingw = 0.0f;
  int32_t texti = getchildidxof(widget, widget->parent);
  if(texti == -1) {
    fprintf(stderr, "reif: cannot find text widget in parent.\n");
    return;
  }
  for(int32_t i = texti + 1; i < widget->parent->num_childs; i++) {
    lf_widget_t* child = widget->parent->childs[i];
    if(!child) continue;
    if(child->sizing_type == LF_SIZING_GROW) continue;
    if (!child->visible) continue;
    rightsiblingw += lf_widget_effective_size(child).x
      + child->props.margin_left + child->props.margin_right;
  }

  float wrap = widget->parent->container.pos.x + widget->parent->container.size.x;
  if(widget->parent->sizing_type == LF_SIZING_FIT_CONTENT || widget->_positioned_absolute_x) {
    wrap = -1.0f;
  }
  text->wrap = text->naturalpass ? -1.0f : wrap;
  if(!(lf_widget_effective_size(widget).x + rightsiblingw > widget->parent->container.size.x + widget->parent->props.padding_right)) {
    wrap = -1.0f;
  }
  vec2s text_pos = (vec2s){
    .x = widget->container.pos.x + widget->props.padding_left, 
    .y =  widget->container.pos.y + widget->props.padding_top 
  };
  lf_text_dimension_t text_dimension = ui->render_get_paragraph_dimension(
    ui->render_state,
    text->label,
    text_pos,
    text->font.font,
    (lf_paragraph_props_t){
      .wrap = text->wrap, 
      .align = widget->props.text_align
    }
  );
  if((text->_text_dimension.width != text_dimension.width || 
      text->_text_dimension.height != text_dimension.height)) {
    widget->_changed_size = true;
  }
  if(!widget->_fixed_width)
    text->base.container.size.x = text_dimension.width; 
  if(!widget->_fixed_height)
    text->base.container.size.y = text_dimension.height;

  text->_text_dimension = text_dimension;
}

void
_text_size_calc(
  lf_ui_state_t* ui, 
  lf_widget_t* widget
) {
  if(widget->type != LF_WIDGET_TYPE_TEXT || !widget) return;
  if(!widget->_needs_size_calc) return;
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
    .x = widget->rendered_pos.x + widget->props.padding_left, 
    .y =  widget->rendered_pos.y + widget->props.padding_top 
  };

  ui->render_rect(
    ui->render_state, 
    widget->rendered_pos,
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
        .wrap = text->wrap, 
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

  text->font = font;
  text->label = strdup(label);

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
    LF_WIDGET_TYPE_TEXT,
    LF_SCALE_CONTAINER(text_dimension.width, text_dimension.height),
    props,
    _text_render, 
    NULL,
    NULL,
    _text_size_calc
  );
  text->_changed_label = true;
  text->base.props.text_color = parent->props.text_color;
  text->base.layout_type = LF_LAYOUT_NONE;
  text->base._changed_size = true;
  lf_widget_add_child(parent, (lf_widget_t*)text);


  _recalculate_label(ui, text);
  text->base._needs_size_calc = false;

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
  text->base._needs_size_calc = false;
}


void 
lf_text_set_label(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    const char* label) 
{
  if (text->label) {
    free(text->label);  
  }

  if (label) {
    text->label = strdup(label);  
  } else {
    text->label = NULL;
  }

  text->base._changed_size = true;
  ui->needs_render = true;

  lf_widget_t* shaping = lf_widget_flag_for_layout(ui, &text->base);
  lf_widget_shape(ui, shaping);
}
