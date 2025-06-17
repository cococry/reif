#pragma once 

#include "../widget.h"
#include "../ui_core.h"
#include "text.h"

typedef void (*lf_input_func_t)(
    lf_ui_state_t* ui,
    lf_widget_t* widget);

  
typedef struct {
  lf_widget_t base;

  lf_input_func_t on_focus;
  lf_input_func_t on_unfocus;
  lf_input_func_t on_type;

  bool _hovered, _focused;
  bool _show_cursor;
  bool _blink_held;
  lf_timer_t* _blinktimer;

  char* buf;

  lf_text_t* text_widget;
} lf_input_t;

lf_input_t* lf_input_create(
    lf_ui_state_t* ui,
    lf_widget_t* parent,
    char* buf);
