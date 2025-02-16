#pragma once

#include <stdint.h>

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
  WinEventPropertyX11   = 1 << 9
} lf_event_type_t;

typedef struct {
  lf_event_type_t type;
  int32_t width, height;
  uint32_t keycode, keystate;
  uint32_t button;
  int16_t x, y;
  char keyunicode[32];
  uint32_t atom;
} lf_event_t;
