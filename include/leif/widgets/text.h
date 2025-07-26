#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  lf_widget_t base;

  char* label;

  lf_mapped_font_t font;

  lf_text_dimension_t _text_dimension;
  bool _changed_font_size, _changed_label;
  float wrap;
} lf_text_t;


lf_text_t* lf_text_create_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label,
    lf_mapped_font_t font);

void lf_text_set_font(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    const char* family_name,
    lf_font_style_t style,
    uint32_t pixel_size);

void lf_text_set_label(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    const char* label);
