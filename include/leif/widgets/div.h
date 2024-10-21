#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  lf_widget_t base;

  float fixed_width, fixed_height;

  uint32_t flags;
} lf_div_t;

lf_div_t* lf_div_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);

void lf_div_set_fixed_width(
    lf_div_t* div,
    float width);

void lf_div_set_fixed_height(
    lf_div_t* div,
    float height);

