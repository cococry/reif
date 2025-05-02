#include "../include/leif/task.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdatomic.h>

#define MAX_TASKS 256

static lf_task_t s_tasks[MAX_TASKS]; 
static atomic_int s_head = 0;
static atomic_int s_tail = 0;
static pthread_mutex_t s_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

bool 
lf_task_enqueue(lf_task_cb_t cb, void* userdata) {
  pthread_mutex_lock(&s_queue_mutex);
  int next_tail = (s_tail + 1) % MAX_TASKS;
  if (next_tail == s_head) {
    // Queue full
    pthread_mutex_unlock(&s_queue_mutex);
    return false;
  }
  s_tasks[s_tail].cb = cb;
  s_tasks[s_tail].userdata = userdata;
  s_tail = next_tail;
  pthread_mutex_unlock(&s_queue_mutex);
  return true;
}

void lf_task_flush_all_tasks(void) {
  while (true) {
    pthread_mutex_lock(&s_queue_mutex);
    if (s_head == s_tail) {
      pthread_mutex_unlock(&s_queue_mutex);
      break;
    }

    lf_task_t task = s_tasks[s_head];
    s_head = (s_head + 1) % MAX_TASKS;
    pthread_mutex_unlock(&s_queue_mutex);

    task.cb(task.userdata);
    // now loop again to check if any new tasks were added
  }
}
