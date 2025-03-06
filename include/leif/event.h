#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
  WinEventNone          = 1 << 0,
  WinEventClose         = 1 << 1,
  WinEventRefresh       = 1 << 2,
  WinEventResize        = 1 << 3,
  WinEventKeyPress      = 1 << 4,
  WinEventKeyRelease    = 1 << 5,
  WinEventMouseMove     = 1 << 6,
  WinEventMousePress    = 1 << 7,
  WinEventMouseRelease  = 1 << 8,
  WinEventPropertyX11   = 1 << 9,
  WinEventMouseWheel    = 1 << 10,
} lf_event_type_t;

typedef struct {
  lf_event_type_t type;
  int32_t width, height;
  uint32_t keycode, keystate;
  uint32_t button;
  int16_t x, y;
  int16_t scroll_x, scroll_y;
  int16_t delta_x, delta_y;
  char keyunicode[32];
  uint32_t atom;

  bool handled;
} lf_event_t;
