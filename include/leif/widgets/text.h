#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  lf_widget_t base;

  char* label;
  void* font;

  lf_text_dimension_t _text_dimension;
  bool _changed_font_size;
} lf_text_t;


lf_text_t* lf_text_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label);

lf_text_t* lf_text_create_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label,
    lf_font_t font);

void lf_text_set_font(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    void* font);

void lf_text_set_font_size(
    lf_ui_state_t* ui, 
    lf_text_t* text,
    uint32_t size);

