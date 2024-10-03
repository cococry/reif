#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef void (*lf_button_on_click_func_t)(
    lf_ui_state_t* ui,
    lf_widget_t* widget); 

typedef struct {
  lf_widget_t base;

  char* label;
  void* font;

  lf_button_on_click_func_t on_click;

  lf_color_t text_color;
} lf_button_t;

lf_button_t* lf_button_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);

lf_button_t* lf_button_create_with_label(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label);

void lf_button_set_font(
    lf_ui_state_t* ui, 
    lf_button_t* button,
    void* font);
