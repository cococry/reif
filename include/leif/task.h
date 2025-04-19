#pragma once

#include <stdbool.h>

typedef void (*lf_task_cb_t)(void*);

typedef struct {
  lf_task_cb_t cb;
  void* userdata;
} lf_task_t;


bool lf_task_enqueue(lf_task_cb_t cb, void* userdata);

void lf_task_flush_all_tasks(void);
