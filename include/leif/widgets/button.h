#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  lf_widget_t* base;

  char* label;
} lf_button_t;

lf_button_t* lf_button(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    const char* label);
