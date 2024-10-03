#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef enum {
  LayoutVetical = 0,
  LayoutHorizontalLine,
} lf_div_layout_type_t;

typedef struct {
  lf_widget_t base;

  lf_div_layout_type_t layout;
} lf_div_t;

lf_div_t* lf_div_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);

void lf_div_destroy(lf_div_t* div);

void lf_div_set_layout(
    lf_div_t* div, 
    lf_div_layout_type_t layout);
