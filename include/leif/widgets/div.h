#pragma once

#include "../widget.h"
#include "../ui_core.h"

typedef enum {
  DivAdjustCenterHorizontal = 1 << 0,
  DivAdjustCenterVertical   = 1 << 1
} lf_div_flag_t;

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

void lf_div_set_flag(
    lf_div_t* div, 
    lf_div_flag_t flag
    );

void lf_div_unset_flag(
    lf_div_t* div, 
    lf_div_flag_t flag
    );

bool lf_div_has_flag(
    lf_div_t* div,
    lf_div_flag_t flag
    );
