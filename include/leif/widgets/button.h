#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef void (*lf_button_func_t)(
    lf_ui_state_t* ui,
    lf_widget_t* widget); 


typedef struct {
  lf_widget_t base;

  char* label;
  void* font;

  lf_button_func_t on_click;
  lf_button_func_t on_enter, on_leave;

  lf_color_t text_color;

  bool _changed_font_size;
  bool _hovered;
} lf_button_t;

lf_button_t* lf_button_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);

lf_button_t* lf_button_create_with_label(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label);

lf_button_t* lf_button_create_with_label_ex(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label,
    lf_font_t font);

void lf_button_set_font(
    lf_ui_state_t* ui, 
    lf_button_t* button,
    void* font);

void lf_button_set_font_size(
    lf_ui_state_t* ui, 
    lf_button_t* button,
    uint32_t size);
