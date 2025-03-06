
#include <stdint.h>
#ifdef LF_X11
#include <X11/X.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "../../include/leif/win.h"
#include "../../include/leif/ui_core.h"
#include "../../include/leif/event.h"
#include "../../include/leif/widget.h"
#include "../../include/leif/util.h"

#define MAX_WINDOWS 16

typedef struct {
  lf_win_mouse_press_func ev_mouse_press_cb;
  lf_win_mouse_release_func ev_mouse_release_cb;
  lf_win_refresh_func ev_refresh_cb;
  lf_win_resize_func ev_resize_cb;
  lf_win_close_func ev_close_cb;
  lf_win_mouse_move_func ev_move_cb;
  Window win;
} window_callbacks_t;

static lf_windowing_event_func windowing_event_cb = NULL;

static Display *display = NULL;
static window_callbacks_t window_callbacks[MAX_WINDOWS];
static uint32_t n_windows = 0;
static lf_ui_state_t* ui = NULL;
static lf_event_type_t current_event = WinEventNone;
static Atom wm_protocols_atom, wm_delete_window_atom;
static XVisualInfo* glvis; 
static GLXContext glcontext;

static int last_mouse_x = 0;
static int last_mouse_y = 0;

static void handle_event(XEvent *event);

static lf_window_t create_window(uint32_t width, uint32_t height, const char* title, uint32_t flags, lf_windowing_hint_kv_t* hints, uint32_t nhints);


void 
handle_event(XEvent *event) {
  lf_event_t ev = {0};
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == event->xany.window) {
      switch (event->type) {
        case Expose:
          ev.type = WinEventRefresh;
          current_event = ev.type;
          lf_widget_handle_event(ui, ui->root, &ev);
          if (window_callbacks[i].ev_refresh_cb) {
            window_callbacks[i].ev_refresh_cb(
              ui, 
              (lf_window_t)event->xany.window);
          }
          break;
        case ConfigureNotify:
          ev.type = WinEventResize; 
          ev.width = event->xconfigure.width;
          ev.height = event->xconfigure.height;
          current_event = ev.type;
          lf_widget_handle_event(ui, ui->root, &ev);
          if (window_callbacks[i].ev_resize_cb)
            window_callbacks[i].ev_resize_cb(
              ui, 
              (lf_window_t)event->xany.window, 
              event->xconfigure.width, event->xconfigure.height);
          break;
        case ButtonPress:
          if(event->xbutton.button == Button2) { // Scrollwheel press
            ev.type = WinEventMouseWheel;
            ev.x = 0;
            ev.y = 0;
          }
          if(event->xbutton.button == Button4) { // Scrollwheel up
            ev.type = WinEventMouseWheel;
            ev.scroll_x = 0;
            ev.scroll_y = 1;
          } else if(event->xbutton.button == Button5){ // Scrollwheel down
            ev.type = WinEventMouseWheel;
            ev.scroll_x = 0;
            ev.scroll_y = -1;
          } else if(event->xbutton.button == 6){ // Scrollwheel left
            ev.type = WinEventMouseWheel;
            ev.scroll_x = -1;
            ev.scroll_y = 0;
          } else if(event->xbutton.button == 7){ // SCrollwheel right
            ev.type = WinEventMouseWheel;
            ev.scroll_x = 1;
            ev.scroll_y = 0;
          } else {
            ev.type = WinEventMousePress;
          }
          ev.button = event->xbutton.button;

          if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
          if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
          ev.x = event->xbutton.x;
          ev.y = event->xbutton.y;
          ev.delta_x = last_mouse_x - event->xbutton.x;
          ev.delta_y = last_mouse_y - event->xbutton.y;
          last_mouse_x = event->xbutton.x;
          last_mouse_y = event->xbutton.y;

          current_event = ev.type;
          lf_widget_handle_event(ui, ui->root, &ev);
          if (window_callbacks[i].ev_mouse_press_cb)
            window_callbacks[i].ev_mouse_press_cb(
              ui, 
              (lf_window_t)event->xany.window, 
              event->xbutton.button);
          break;
        case ButtonRelease:
          ev.button = event->xbutton.button;
          ev.type = WinEventMouseRelease; 
          if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
          if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
          ev.x = event->xbutton.x;
          ev.y = event->xbutton.y;
          ev.delta_x = last_mouse_x - event->xbutton.x;
          ev.delta_y = last_mouse_y - event->xbutton.y;
          last_mouse_x = event->xbutton.x;
          last_mouse_y = event->xbutton.y;

          current_event = ev.type;
          lf_widget_handle_event(ui, ui->root, &ev);
          if (window_callbacks[i].ev_mouse_release_cb)
            window_callbacks[i].ev_mouse_release_cb(
              ui, 
              (lf_window_t)event->xany.window,
              event->xbutton.button);
          break;
        case MotionNotify:
          if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
          if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
          ev.x = (uint16_t)event->xmotion.x;
          ev.y = (uint16_t)event->xmotion.y;
          ev.delta_x = last_mouse_x - event->xmotion.x;
          ev.delta_y = last_mouse_y - event->xmotion.y;
          last_mouse_x = event->xmotion.x;
          last_mouse_y = event->xmotion.y;

          ev.type = WinEventMouseMove;
          current_event = ev.type; 
          lf_widget_handle_event(ui, ui->root, &ev);
          if (window_callbacks[i].ev_move_cb)
            window_callbacks[i].ev_move_cb(
              ui, 
              (lf_window_t)event->xany.window, 
              event->xmotion.x, event->xmotion.y);
          break;
        case ClientMessage:
          if (event->xclient.message_type == wm_protocols_atom && 
            (Atom)event->xclient.data.l[0] == wm_delete_window_atom) {
            ev.type = WinEventClose;
            current_event = ev.type;
            lf_widget_handle_event(ui, ui->root, &ev);
            if (window_callbacks[i].ev_close_cb) {
              window_callbacks[i].ev_close_cb(ui, (lf_window_t)event->xany.window);
            }
          }
          break;
    }
    }
  }
}

lf_window_t
create_window(
  uint32_t width, 
  uint32_t height, 
  const char* title,
  uint32_t flags, 
  lf_windowing_hint_kv_t* hints,
  uint32_t nhints) {
  Window root = DefaultRootWindow(display);
  int screen = DefaultScreen(display);

  uint32_t winpos_x = 0, winpos_y = 0;

  for(uint32_t i = 0; i < nhints; i++) {
    if(hints[i].key == LF_WINDOWING_HINT_POS_X) {
      winpos_x = hints[i].value;
    }
    else if(hints[i].key == LF_WINDOWING_HINT_POS_Y) {
      winpos_y = hints[i].value;
    }
  }
  Window win = XCreateSimpleWindow(display, root, winpos_x, winpos_y, width, height, 0,
                                   BlackPixel(display, screen), BlackPixel(display, screen));

  if(lf_flag_exists(&flags, LF_WINDOWING_X11_OVERRIDE_REDIRECT)) {
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(display, win, CWOverrideRedirect, &attributes);  
  }

  XSelectInput(display, win, StructureNotifyMask | KeyPressMask | KeyReleaseMask |
               ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask);
  XMapWindow(display, win);

  XStoreName(display, win, title);

  if (n_windows + 1 <= MAX_WINDOWS) {
    window_callbacks[n_windows].win = win;
    window_callbacks[n_windows].ev_mouse_press_cb = NULL;
    window_callbacks[n_windows].ev_mouse_release_cb = NULL;
    window_callbacks[n_windows].ev_close_cb = NULL;
    window_callbacks[n_windows].ev_refresh_cb = NULL;
    window_callbacks[n_windows].ev_resize_cb = NULL;
    ++n_windows;
  } else {
    fprintf(stderr, "warning: reached maximum amount of windows to define callbacks for.\n");
  }

  return win;
}

int32_t 
lf_windowing_init(void) {
  display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "reif: cannot open X display.\n");
    return 1;
  }
  wm_protocols_atom = XInternAtom(display, "WM_PROTOCOLS", False);
  wm_delete_window_atom = XInternAtom(display, "WM_DELETE_WINDOW", False);
  int attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

  int screen_num = DefaultScreen(display);
  glvis = glXChooseVisual(display, screen_num, attribs);
  if (!glvis) {
    fprintf(stderr, "reif: cannot choose glX visual.\n");
    return 1;
  }

  glcontext = glXCreateContext(display, glvis, NULL, GL_TRUE); 

  return 0;
}

int32_t 
lf_windowing_terminate(void) {
  XCloseDisplay(display);
  glXDestroyContext(display, glcontext);
  return 0;
}

void
lf_windowing_update(void) {
  current_event = WinEventNone;
}

void
lf_windowing_set_ui_state(lf_ui_state_t* state) {
  ui = state;
}

lf_event_type_t 
lf_windowing_get_current_event(void) {
  return current_event;
}

void 
lf_windowing_next_event(void) {
  XEvent event;
  while (XPending(display)) {
    XNextEvent(display, &event);
    if(windowing_event_cb)
      windowing_event_cb(&event);
    handle_event(&event);
  }
}

void* 
lf_win_get_display(void) {
  return display;
}

lf_window_t 
lf_win_create(uint32_t width, uint32_t height, const char* title) {
  return create_window(width, height, title, 0, NULL, 0); 
}

lf_window_t 
lf_win_create_ex(
  uint32_t width, uint32_t height, 
  const char* title, 
  uint32_t flags, 
  lf_windowing_hint_kv_t* hints, uint32_t nhints) {
  return create_window(width, height, title, flags, hints, nhints); 
}


void 
lf_win_set_title(lf_window_t win, const char* title) {
  XStoreName(display, (Window)win, title);
}

int32_t 
lf_win_make_gl_context(lf_window_t win) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      glXMakeCurrent(display, window_callbacks[i].win, glcontext); 
      return 0;
    }
  }
  return 1;
}

void 
lf_win_swap_buffers(lf_window_t win) {
  glXSwapBuffers(display, (Window)win);
}

void 
lf_win_destroy(lf_window_t win) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      XDestroyWindow(display, window_callbacks[i].win);
      break;
    }
  }
}

vec2s 
lf_win_cursor_pos(lf_window_t win) {
  Window root, child;
  int root_x, root_y, win_x, win_y;
  unsigned int mask;
  XQueryPointer(display, (Window)win, &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);

  return (vec2s){
    .x = (float)win_x,
    .y = (float)win_y
  };
}

void 
lf_win_set_close_cb(lf_window_t win, lf_win_close_func close_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_close_cb = close_cb;
      break;
    }
  }
}

void 
lf_win_set_refresh_cb(lf_window_t win, lf_win_refresh_func refresh_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_refresh_cb = refresh_cb;
      break;
    }
  }
}

void 
lf_win_set_resize_cb(lf_window_t win, lf_win_resize_func resize_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_resize_cb = resize_cb;
      break;
    }
  }
}

void
lf_win_set_mouse_press_cb(lf_window_t win, lf_win_mouse_press_func mouse_press_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_mouse_press_cb = mouse_press_cb;
      break;
    }
  }
}

void
lf_win_set_mouse_release_cb(lf_window_t win, lf_win_mouse_release_func mouse_release_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_mouse_release_cb = mouse_release_cb;
      break;
    }
  }
}

void 
lf_win_set_mouse_move_cb(lf_window_t win, lf_win_mouse_move_func mouse_move_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_move_cb = mouse_move_cb;
      break;
    }
  }
}


vec2s 
lf_win_get_size(lf_window_t win) {
  XWindowAttributes attr;
  XGetWindowAttributes(display, (Window)win, &attr);
  return (vec2s){
    .x = (float)attr.width,
    .y = (float)attr.height
  };
}

int32_t
lf_win_get_refresh_rate(lf_window_t win) {
  (void)win;
  // Placeholder as X11 doesn't provide refresh rate directly.
  return 144; // Default refresh rate.
}

void 
lf_windowing_set_event_cb(lf_windowing_event_func cb) {
  windowing_event_cb = cb;
}

#endif
