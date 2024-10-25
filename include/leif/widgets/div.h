#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  lf_widget_t base;
  uint32_t flags;

  float _fixed_width, _fixed_height;
  int32_t _column_count;
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

void lf_div_set_column_count(
    lf_div_t* div,
    uint32_t column_count);
