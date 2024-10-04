#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef struct {
  lf_widget_t base;
} lf_div_t;

lf_div_t* lf_div_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent);

void lf_div_destroy(lf_div_t* div);
