#pragma once

#include <stdbool.h>

typedef struct lf_ui_state_t lf_ui_state_t;
typedef struct lf_timer_t lf_timer_t;

typedef void (*lf_timer_finish_func_t)(lf_ui_state_t* ui, lf_timer_t* timer);

struct lf_timer_t {
  float elapsed;
  float duration;
  bool expired, looping;
  bool paused;

  lf_timer_finish_func_t finish_cb;

  void* user_data;
};

void lf_timer_tick(lf_ui_state_t* ui, lf_timer_t* timer, float dt, bool auto_destruct);
