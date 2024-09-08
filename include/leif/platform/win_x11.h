#pragma once

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#include <stdint.h>

typedef struct {
  Window win;
  uint32_t width, height;
  char* title;
  
  XIC xinputcontext;
} lf_window_t;
