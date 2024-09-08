#pragma once

#include <stdint.h>

typedef enum {
  WinEventNone = 0,
  WinEventClose,
  WinEventExpose,
  WinEventKeyPress,
  WinEventKeyRelease,
  WinEventMouseMove,
  WinEventMousePress
} lf_event_type_t;

typedef struct {
  lf_event_type_t type;
  int32_t width, height;
  uint32_t keycode, keystate;
  uint32_t button;
  int16_t x, y;
  char keyunicode[32];
} lf_event_t;
