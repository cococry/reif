#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  LF_EVENT_NONE = 1 << 0,
  LF_EVENT_WINDOW_CLOSE = 1 << 1,
  LF_EVENT_WINDOW_REFRESH = 1 << 2,
  LF_EVENT_WINDOW_RESIZE = 1 << 3,
  LF_EVENT_KEY_PRESS = 1 << 4,
  LF_EVENT_KEY_RELEASE = 1 << 5,
  LF_EVENT_MOUSE_MOVE = 1 << 6,
  LF_EVENT_MOUSE_PRESS = 1 << 7,
  LF_EVENT_MOUSE_RELEASE = 1 << 8,
  LF_EVENT_MOUSE_WHEEL = 1 << 9,
#ifdef LF_X11
  LF_EVENT_X11_PROPERTY = 1 << 10,
#endif 
} lf_event_type_t;

typedef enum {
  LF_KEY_ACTION_PRESS = 0,
  LF_KEY_ACTION_RELEASE = 0,
} lf_event_key_action;

typedef struct {
  lf_event_type_t type;
  int32_t width, height;
  uint32_t keycode, keyscancode, keymods;
  lf_event_key_action keyaction;
  uint32_t button;
  int16_t x, y;
  int16_t scroll_x, scroll_y;
  int16_t delta_x, delta_y;
  char keyunicode[32];
  uint32_t atom;

  bool handled;
} lf_event_t;
