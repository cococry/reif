
#ifdef LF_X11
#include <GL/gl.h>
#include <stdint.h>
#include <unistd.h>
#include <X11/X.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <assert.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
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
  lf_win_key_func ev_key_cb;
  lf_win_char_func ev_char_cb;
  lf_windowing_event_func windowing_event_cb;

  lf_win_mouse_move_func ev_move_cb;
  Window win;
  int32_t win_width, win_height;
  lf_ui_state_t* ui;
  GLXContext glcontext;
  uint32_t flags;
  lf_windowing_hint_kv_t* hints;
  uint32_t nhints;

  XIC xinputcontext;
} window_callbacks_t;

typedef struct {
  Visual *visual;
  VisualID visualid;
  int screen;
  unsigned int depth;
  int class;
  unsigned long red_mask;
  unsigned long green_mask;
  unsigned long blue_mask;
  int colormap_size;
  int bits_per_rgb;
} visual_info_t;


static Display *display = NULL;
static window_callbacks_t window_callbacks[MAX_WINDOWS];
static uint32_t n_windows = 0;
static lf_event_type_t current_event = LF_EVENT_NONE;
static float last_motion_time = 0;
static Atom 
net_wm_state,
net_wm_state_above,
net_wm_state_below,
net_wm_window_type, 
net_wm_window_type_normal, 
net_wm_ping,
net_wm_pid,
wm_protocols, 
wm_delete_window,
motif_wm_hints;
static GLXContext share_gl_context = 0;
static XIM xim;

static int last_mouse_x = 0;
static int last_mouse_y = 0;

static window_callbacks_t* win_data_from_native(lf_window_t win);

static void handle_event(XEvent *event);

static void handle_key_event(XKeyEvent *event, window_callbacks_t data);

static lf_window_t create_window(uint32_t width, uint32_t height, const char* title, uint32_t flags, lf_windowing_hint_kv_t* hints, uint32_t nhints);

void get_window_size(Display* display, Window window, int32_t* width, int32_t* height) {
  XWindowAttributes attrs;
  if (XGetWindowAttributes(display, window, &attrs)) {
    *width = attrs.width;
    *height = attrs.height;
  } else {
    assert(false && "reif: failed to get window attributes\n");
  }
}

window_callbacks_t* win_data_from_native(lf_window_t win) {
  for(uint32_t i = 0; i < n_windows; i++) {
    if(window_callbacks[i].win == win) {
      return &window_callbacks[i];
    }
  }
  return NULL;
}

bool boolean_hint_set(lf_windowing_hint_kv_t* hints, uint32_t nhints, lf_window_hint_t key) {
  for(uint32_t i = 0; i < nhints; i++) {
    if(hints[i].key == key && hints[i].value == true) return true;
  }
  return false;
}

void 
handle_event(XEvent *event) {
  lf_event_t ev = {0};
  for (uint32_t i = 0; i < n_windows; ++i) {
    window_callbacks_t win_data = window_callbacks[i]; 
    if(event->xany.window != win_data.win) continue; 
    switch (event->type) {
      case Expose:
        ev.type = LF_EVENT_WINDOW_REFRESH;
        current_event = ev.type;
        if(win_data.ui) { 
          lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
        }
        if (window_callbacks[i].ev_refresh_cb && win_data.ui) {
          window_callbacks[i].ev_refresh_cb(
            win_data.ui, 
            (lf_window_t)event->xany.window);
        }
        break;
      case ConfigureNotify:
        int32_t actual_win_w, actual_win_h;
        get_window_size(display, window_callbacks[i].win, 
                        &actual_win_w, 
                        &actual_win_h);
        ev.type = LF_EVENT_WINDOW_RESIZE;
        ev.width = actual_win_w;
        ev.height = actual_win_h;
        current_event = ev.type;

        window_callbacks[i].win_width = actual_win_w;
        window_callbacks[i].win_height = actual_win_h; 

        if (win_data.ui) {
          lf_win_make_gl_context(win_data.win);
          lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
        }

        // Call the resize callback if defined
        if (window_callbacks[i].ev_resize_cb && win_data.ui) {
          window_callbacks[i].ev_resize_cb(
            win_data.ui, 
            (lf_window_t)event->xany.window, 
            actual_win_h, actual_win_w 
          );
        }
        break;
      case ButtonPress:
        ev.type = LF_EVENT_MOUSE_WHEEL; // Default to Scrollwheel 
        if(event->xbutton.button == Button2) { // Scrollwheel press
          ev.scroll_x = 0;
          ev.scroll_y = 0;
        }
        if(event->xbutton.button == Button4) { // Scrollwheel up
          ev.type = LF_EVENT_MOUSE_WHEEL;
          ev.scroll_x = 0;
          ev.scroll_y = 1;
        } else if(event->xbutton.button == Button5){ // Scrollwheel down
          ev.type = LF_EVENT_MOUSE_WHEEL;
          ev.scroll_x = 0;
          ev.scroll_y = -1;
        } else if(event->xbutton.button == 6){ // Scrollwheel left
          ev.type = LF_EVENT_MOUSE_WHEEL;
          ev.scroll_x = -1;
          ev.scroll_y = 0;
        } else if(event->xbutton.button == 7){ // SCrollwheel right
          ev.type = LF_EVENT_MOUSE_WHEEL;
          ev.scroll_x = 1;
          ev.scroll_y = 0;
        } else {
          ev.type = LF_EVENT_MOUSE_PRESS;
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
        if(win_data.ui)
          lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
        if (window_callbacks[i].ev_mouse_press_cb && win_data.ui)
          window_callbacks[i].ev_mouse_press_cb(
            win_data.ui, 
            (lf_window_t)event->xany.window, 
            event->xbutton.button);
        break;
      case ButtonRelease:
        ev.button = event->xbutton.button;
        ev.type = LF_EVENT_MOUSE_RELEASE; 
        if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
        if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
        ev.x = event->xbutton.x;
        ev.y = event->xbutton.y;
        ev.delta_x = last_mouse_x - event->xbutton.x;
        ev.delta_y = last_mouse_y - event->xbutton.y;
        last_mouse_x = event->xbutton.x;
        last_mouse_y = event->xbutton.y;

        current_event = ev.type;
        if(win_data.ui)
          lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
        if (window_callbacks[i].ev_mouse_release_cb && win_data.ui)
          window_callbacks[i].ev_mouse_release_cb(
            win_data.ui, 
            (lf_window_t)event->xany.window,
            event->xbutton.button);
        break;
      case MotionNotify: {
        uint32_t curtime = event->xmotion.time;
        if((curtime - last_motion_time) <= (1000.0 / 60)) {
          break;
        }
        last_motion_time = curtime;
        if(last_mouse_x == 0) last_mouse_x = event->xbutton.x;
        if(last_mouse_y == 0) last_mouse_y = event->xbutton.y;
        ev.x = (uint16_t)event->xmotion.x;
        ev.y = (uint16_t)event->xmotion.y;
        ev.delta_x = last_mouse_x - event->xmotion.x;
        ev.delta_y = last_mouse_y - event->xmotion.y;
        last_mouse_x = event->xmotion.x;
        last_mouse_y = event->xmotion.y;

        ev.type = LF_EVENT_MOUSE_MOVE;
        current_event = ev.type; 
        if(win_data.ui)
          lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
        if (window_callbacks[i].ev_move_cb && win_data.ui)
          window_callbacks[i].ev_move_cb(
            win_data.ui, 
            (lf_window_t)event->xany.window, 
            event->xmotion.x, event->xmotion.y);
        break;
      }
      case ClientMessage:
        if (event->xclient.message_type == wm_protocols) {
          Atom protocol = (Atom)event->xclient.data.l[0]; 
          if(protocol == wm_delete_window) {
            ev.type = LF_EVENT_WINDOW_CLOSE;
            current_event = ev.type;
            if(win_data.ui)
              lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
            if (window_callbacks[i].ev_close_cb && win_data.ui) {
              window_callbacks[i].ev_close_cb(win_data.ui, (lf_window_t)event->xany.window);
            }
            glXDestroyContext(display, window_callbacks[i].glcontext);
          } else if(protocol == net_wm_ping) {
            XEvent reply = *event;
            reply.xclient.window = DefaultRootWindow(display);
            XSendEvent(display, DefaultRootWindow(display),
                       False,
                       SubstructureNotifyMask | SubstructureRedirectMask,
                       &reply);
          }
        }
        break;
      case LeaveNotify: 
        {
          XWindowAttributes wa;
          XGetWindowAttributes(display, event->xcrossing.window, &wa);

          lf_widget_t* active_widget = lf_widget_from_id(
            win_data.ui, 
            win_data.ui->root, 
            win_data.ui->active_widget_id);
          if (wa.override_redirect && (!active_widget || active_widget->type != LF_WIDGET_TYPE_SLIDER)) {
            ev.x = -1; 
            ev.y = -1; 
            ev.delta_x = 0; 
            ev.delta_y = 0; 
            ev.type = LF_EVENT_MOUSE_MOVE;
            current_event = ev.type; 
            if(win_data.ui)
              lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
            if (window_callbacks[i].ev_move_cb && win_data.ui)
              window_callbacks[i].ev_move_cb(
                win_data.ui, 
                (lf_window_t)event->xany.window, 
                -1, -1);
          }
          break;
        }
      case KeyPress:
      case KeyRelease:
        {
          handle_key_event(&event->xkey, window_callbacks[i]);
        }
    }
  }
}

void handle_key_event(XKeyEvent *event, window_callbacks_t data) {
  KeySym keysym = XLookupKeysym(event, 0);
  int32_t key = (int32_t)keysym;                 
  int32_t scancode = event->keycode;             
  int32_t action = (event->type == KeyPress) ? LF_KEY_ACTION_PRESS : LF_KEY_ACTION_RELEASE;  
  int32_t mods = 0;

  if (event->state & ShiftMask)   mods |= (1 << 0);  // MOD_SHIFT
  if (event->state & ControlMask) mods |= (1 << 1);  // MOD_CONTROL
  if (event->state & Mod1Mask)    mods |= (1 << 2);  // MOD_ALT
  if (event->state & Mod4Mask)    mods |= (1 << 3);  // MOD_SUPER

  // --- Handle raw key press/release
  lf_event_t key_ev = {0};
  key_ev.keycode = key;
  key_ev.keyscancode = scancode;
  key_ev.keyaction = action;
  key_ev.keymods = mods;
  key_ev.type = (event->type == KeyPress) ? LF_EVENT_KEY_PRESS : LF_EVENT_KEY_RELEASE;

  current_event = key_ev.type;
  if (data.ui)
    lf_widget_handle_event(data.ui, data.ui->root, &key_ev);
  if (data.ev_key_cb && data.ui)
    data.ev_key_cb(data.ui, (lf_window_t)event->window, key, scancode, action, mods);

  if (event->type == KeyPress) {
    char utf8[32] = {0};
    Status status;
    KeySym dummy_keysym;
    int len = Xutf8LookupString(data.xinputcontext, event, utf8, sizeof(utf8) - 1, &dummy_keysym, &status);

    if (len > 0 && (status == XLookupChars || status == XLookupBoth)) {
      lf_event_t char_ev = {0};
      char_ev.type = LF_EVENT_TYPING_CHAR;
      memcpy(char_ev.charutf8, utf8, 5);  // copy only first 5 bytes max
      current_event = char_ev.type;

      if (data.ui)
        lf_widget_handle_event(data.ui, data.ui->root, &char_ev);
      if (data.ev_char_cb && data.ui)
        data.ev_char_cb(data.ui, (lf_window_t)event->window, char_ev.charutf8, len);
    }
  }
}

typedef GLXContext (*glXCreateContextAttribsARBProc)(
    Display*, GLXFBConfig, GLXContext, Bool, const int*);

lf_window_t create_window(
  uint32_t width, 
  uint32_t height, 
  const char* title,
  uint32_t flags, 
  lf_windowing_hint_kv_t* hints,
  uint32_t nhints) {

  Display* dpy = XOpenDisplay(NULL);
  if (!dpy) {
    fprintf(stderr, "Cannot open display\n");
    return NULL;
  }

  static int fbAttribs[] = {
    GLX_X_RENDERABLE,  True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE,      8,
    GLX_GREEN_SIZE,    8,
    GLX_BLUE_SIZE,     8,
    GLX_ALPHA_SIZE,    8,
    None
  };

  int fbcount;
  GLXFBConfig* fbc = glXChooseFBConfig(dpy, DefaultScreen(dpy), fbAttribs, &fbcount);
  if (!fbc) {
    fprintf(stderr, "Failed to get FBConfig\n");
    return NULL;
  }

  XVisualInfo* vi = glXGetVisualFromFBConfig(dpy, fbc[0]);
  if (!vi) {
    fprintf(stderr, "No appropriate visual found\n");
    return NULL;
  }

  XSetWindowAttributes swa;
  swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
  swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

  Window win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 
                             0, 0, width, height, 0, vi->depth, InputOutput,
                             vi->visual,
                             CWColormap | CWEventMask, &swa);

  XStoreName(dpy, win, title);
  XMapWindow(dpy, win);

  // Get modern GL context creation function
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
      (glXCreateContextAttribsARBProc)
      glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

  if (!glXCreateContextAttribsARB) {
    fprintf(stderr, "glXCreateContextAttribsARB not supported\n");
    return NULL;
  }

  int context_attribs[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 2,
    GLX_CONTEXT_MINOR_VERSION_ARB, 1,
    GLX_CONTEXT_PROFILE_MASK_ARB,  GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
  };

  GLXContext ctx = glXCreateContextAttribsARB(dpy, fbc[0], 0, True, context_attribs);
  if (!ctx) {
    fprintf(stderr, "Failed to create OpenGL context\n");
    return NULL;
  }

  XFree(vi);
  XFree(fbc);

  if (!glXMakeCurrent(dpy, win, ctx)) {
    fprintf(stderr, "Failed to make OpenGL context current\n");
    return NULL;
  }

  lf_window_t new_window;
  new_window.dpy = dpy;
  new_window.win = win;
  new_window.ctx = ctx;
  return new_window;
}

typedef void (*glXSwapIntervalEXTProc)(Display*, GLXDrawable, int);
int32_t 
lf_windowing_init(void) {
  if(display) return 0;
  display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "reif: cannot open X display.\n");
    return 1;
  }

  XSetLocaleModifiers("");
  xim = XOpenIM(display, NULL, NULL, NULL);
  if (xim == NULL) {
    fprintf(stderr, "txt: cannot open X input method\n");
    return 1;
  }

glXSwapIntervalEXTProc glXSwapIntervalEXT = (glXSwapIntervalEXTProc)glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalEXT");
if (glXSwapIntervalEXT) {
    glXSwapIntervalEXT(display, glXGetCurrentDrawable(), 1);
}
  wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);
  wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
  motif_wm_hints = XInternAtom(display, "_MOTIF_WM_HINTS", False);
  net_wm_state = XInternAtom(display, "_NET_WM_STATE", False);
  net_wm_state_above = XInternAtom(display, "_NET_WM_STATE_ABOVE", False);
  net_wm_state_below = XInternAtom(display, "_NET_WM_STATE_BELOW", False);
  net_wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
  net_wm_ping = XInternAtom(display, "_NET_WM_PING", False);
  net_wm_pid = XInternAtom(display, "_NET_WM_PID", False);
  net_wm_window_type_normal = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", False);

  return 0;
}

int32_t 
lf_windowing_terminate(void) {
  XCloseDisplay(display);
  return 0;
}

void
lf_windowing_update(void) {
  current_event = LF_EVENT_NONE;
}

void
lf_win_set_ui_state(lf_window_t win, lf_ui_state_t* state) {
  window_callbacks_t* data;
  if(!(data = win_data_from_native(win))) return;
  data->ui = state;
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
    XFilterEvent(&event, None);  
    for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].windowing_event_cb) {
        window_callbacks[i].windowing_event_cb(&event, window_callbacks[i].ui);
      }
    }
    handle_event(&event);
  }
}

void* 
lf_win_get_x11_display(void) {
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
  window_callbacks_t* data = win_data_from_native(win);
  if(!data) return 1;
  glXMakeCurrent(display, win, data->glcontext);
  return 0;
}

void 
lf_win_swap_buffers(lf_window_t win) {
  glXSwapBuffers(display, win);
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

void 
lf_win_set_event_cb(lf_window_t win, lf_windowing_event_func cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].windowing_event_cb = cb;
      break;
    }
  }
}

void 
lf_win_set_key_cb(lf_window_t win, lf_win_key_func cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_key_cb = cb;
      break;
    }
  }
}

void 
lf_win_set_typing_char_cb(lf_window_t win, lf_win_char_func cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == (Window)win) {
      window_callbacks[i].ev_char_cb = cb;
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


int32_t lf_win_get_refresh_rate(lf_window_t win) {
  return 144;
}

void 
lf_win_hide(lf_window_t win) {
  XUnmapWindow(display, win);
  XFlush(display);
} 

void 
lf_win_show(lf_window_t win) {
  XMapWindow(display, win);
  XFlush(display);
}

void 
lf_win_set_width(lf_window_t win, float width) {
  XResizeWindow(display, win, width, lf_win_get_size(win).y);
}

void 
lf_win_set_height(lf_window_t win, float height) {
  XResizeWindow(display, win, lf_win_get_size(win).x, (uint32_t)height);
}

#endif
