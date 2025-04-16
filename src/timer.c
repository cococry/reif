#include "../include/leif/timer.h"
#include <stdlib.h>
#include <stdio.h>

void 
lf_timer_tick(lf_ui_state_t* ui, lf_timer_t* timer, float dt, bool auto_destruct) {
  if(!timer) return;
  timer->elapsed += dt;
  if(timer->elapsed >= timer->duration) {
    if(timer->finish_cb)
      timer->finish_cb(ui, timer);
  }
}
