
#ifdef LF_X11
#include <GL/gl.h>
#include <stdint.h>
#include <unistd.h>
#include <X11/X.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>
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

static bool wait_events = true;
static Cursor cursors[LF_CURSOR_COUNT];

static int pipe_fds[2]; 

static window_callbacks_t* win_data_from_native(lf_window_t win);

static void handle_event(XEvent *event);

static void handle_key_event(XKeyEvent *event, window_callbacks_t data);

static lf_window_t create_window(uint32_t width, uint32_t height, const char* title, uint32_t flags, lf_windowing_hint_kv_t* hints, uint32_t nhints);

void 
lf_windowing_wake_up_event_loop(void) {
  write(pipe_fds[1], "x", 1); 
}

void 
drain_pipe() {
  char buf[64];
  read(pipe_fds[0], buf, sizeof(buf));
}

void 
init_pipe() {
  pipe(pipe_fds);
  fcntl(pipe_fds[0], F_SETFL, O_NONBLOCK);
  fcntl(pipe_fds[1], F_SETFL, O_NONBLOCK);
}

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
        if (event->xconfigure.width == window_callbacks[i].win_width &&
          event->xconfigure.height == window_callbacks[i].win_height) {
          break;  // no need to process the event if the size is the same
        }

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
          if (protocol == wm_delete_window) {
            ev.type = LF_EVENT_WINDOW_CLOSE;
            current_event = ev.type;

            if (win_data.ui) {
              lf_widget_handle_event(win_data.ui, win_data.ui->root, &ev);
            }

            if (window_callbacks[i].ev_close_cb && win_data.ui) {
              window_callbacks[i].ev_close_cb(win_data.ui, (lf_window_t)event->xany.window);
            }

            glXDestroyContext(display, window_callbacks[i].glcontext);
            XDestroyWindow(display, window_callbacks[i].win);  
            for (int j = i; j < n_windows - 1; j++) {
              window_callbacks[j] = window_callbacks[j + 1];
            }
            n_windows--;
            i--;
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
      memcpy(char_ev.charutf8, utf8, 5);
      char_ev.charutf8len = len;
      current_event = char_ev.type;

      if (data.ui)
        lf_widget_handle_event(data.ui, data.ui->root, &char_ev);
      if (data.ev_char_cb && data.ui)
        data.ev_char_cb(data.ui, (lf_window_t)event->window, char_ev.charutf8, len);
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

  uint32_t winpos_x = 0, winpos_y = 0;
  bool adjusting_pos = false;
  bool transparent_framebuffer = false, decorated = false, 
  visible = true, resizable = true, above = false, below = false;
  uint32_t border_width = 0, border_color = 0;
  for(uint32_t i = 0; i < nhints; i++) {
    if(hints[i].key == LF_WINDOWING_HINT_BORDER_WIDTH) {
      border_width = hints[i].value;
    } 
    if(hints[i].key == LF_WINDOWING_HINT_BORDER_COLOR) {
      border_color = hints[i].value;
    } 
    if(hints[i].key == LF_WINDOWING_HINT_TRANSPARENT_FRAMEBUFFER) {
      transparent_framebuffer = hints[i].value;
    } 
    if(hints[i].key == LF_WINDOWING_HINT_DECORATED) {
      decorated = hints[i].value;
    } 
    if(hints[i].key == LF_WINDOWING_HINT_POS_X) {
      adjusting_pos = true;
      winpos_x = hints[i].value;
    }
    else if(hints[i].key == LF_WINDOWING_HINT_POS_Y) {
      adjusting_pos = true;
      winpos_y = hints[i].value;
    }
    else if(hints[i].key == LF_WINDOWING_HINT_RESIZABLE) {
      resizable = hints[i].value;
    }
    else if(hints[i].key == LF_WINDOWING_HINT_VISIBLE) {
      visible = hints[i].value;
    }  
    else if(hints[i].key == LF_WINDOWING_HINT_ABOVE) {
      above = true;
    }
    else if(hints[i].key == LF_WINDOWING_HINT_BELOW) {
      below = true;
    }
  }

  int screen_num = DefaultScreen(display);

  static int visdata[] = {
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_DOUBLEBUFFER, True,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_ALPHA_SIZE, 1,
    GLX_DEPTH_SIZE, 1,
    None
  };

  int32_t nfb;
  visual_info_t* visual = NULL;
  XRenderPictFormat* pic_fmt;
  GLXFBConfig fbconfig = NULL;  
  GLXFBConfig* fbconfigs = glXChooseFBConfig(display, screen_num, visdata, &nfb);
  for(int i = 0; i < nfb; i++) {
    visual = (visual_info_t*) glXGetVisualFromFBConfig(display, fbconfigs[i]);
    if(!visual)
      continue;

    pic_fmt = XRenderFindVisualFormat(display, visual->visual);
    if(!pic_fmt)
      continue;

    if(pic_fmt->direct.alphaMask > 0) {
      fbconfig = fbconfigs[i];
      break;
    }
  }
  if(!visual) return 0;
  if(!fbconfig) return 0;

  Colormap cmap = XCreateColormap(display, DefaultRootWindow(display), visual->visual, AllocNone);

  int dummy;
  if (!glXQueryExtension(display, &dummy, &dummy)) {
    fprintf(stderr, "reif: OpenGL not supported by X server.\n");
    assert(false);
  }
  GLXContext glcontext = glXCreateNewContext(display, fbconfig, GLX_RGBA_TYPE, share_gl_context, True);
  if(!share_gl_context)
    share_gl_context = glcontext;
  if (!glcontext) {
    fprintf(stderr, "reif: failed to create an OpenGL context.\n");
    assert(false);
  }


  XTextProperty textprop;
  XSizeHints size_hints;
  XWMHints* startup_state;
  Window win;
  if(transparent_framebuffer) {
    XSetWindowAttributes attr;
    if(transparent_framebuffer) {
      attr.colormap = cmap;
      attr.background_pixmap = None;
    }
    attr.border_pixel = border_color;
    attr.event_mask =  
      StructureNotifyMask | KeyPressMask      | KeyReleaseMask    |
      ButtonPressMask     | ButtonReleaseMask | PointerMotionMask | 
      ExposureMask        | LeaveWindowMask;
    int32_t attr_mask;
    attr_mask =
      CWBackPixmap  |
      CWColormap    |
      CWBorderPixel |
      CWEventMask;   
    win = XCreateWindow(display, root, 
                        winpos_x, winpos_y, width, height, border_width,
                        visual->depth, InputOutput,  
                        visual->visual, attr_mask, &attr); 
  } else {
    int screen = DefaultScreen(display);
    win = XCreateSimpleWindow(display, root, winpos_x, winpos_y, width, height, border_width,
                              BlackPixel(display, screen), BlackPixel(display, screen));
    XSelectInput(display, win, StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | LeaveWindowMask);
  }
  textprop.value = (unsigned char*)title;
  textprop.encoding = XA_STRING;
  textprop.format = 8;
  textprop.nitems = strlen(title);

  size_hints.x = winpos_x;
  size_hints.y = winpos_y;
  size_hints.width = width;
  size_hints.height = height;
  size_hints.flags = USPosition|USSize;

  startup_state = XAllocWMHints();
  startup_state->initial_state = NormalState;
  startup_state->flags = StateHint;

  XSetWMProperties(display, win, &textprop, &textprop,
                   NULL, 0,
                   &size_hints, 
                   startup_state,
                   NULL);

  XFree(startup_state);

  {
    Atom states[1];
    uint32_t count = 0;
    if(above) {
      states[count++] = net_wm_state_above; 
    } else if(below) {
      states[count++] = net_wm_state_below; 
    }
    XChangeProperty(display, win,
                    net_wm_state, XA_ATOM, 32,
                    PropModeReplace, (unsigned char*) states, count);
  }

  {
    Atom supported[] = {
      wm_delete_window,
      net_wm_ping 
    };
    XSetWMProtocols(
      display, win,
      supported, sizeof(supported) / sizeof(Atom));

    const long pid = getpid();

    XChangeProperty(
      display,win,
      net_wm_pid, XA_CARDINAL, 32,
      PropModeReplace,
      (unsigned char*) &pid, 1);
  }


  if(lf_flag_exists(&flags, LF_WINDOWING_FLAG_X11_OVERRIDE_REDIRECT)) {
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(display, win, CWOverrideRedirect, &attributes);  
  } else {
    Atom type = net_wm_window_type_normal;
    XChangeProperty(
      display, win,
      net_wm_window_type, XA_ATOM, 32,
      PropModeReplace, (unsigned char*) &type, 1);
  }

  if(visible)
    XMapWindow(display, win);
  else 
    XUnmapWindow(display, win);

  struct
  {
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long input_mode;
    unsigned long status;
  } decoration_hints = {0};

  static const uint32_t MWM_HINTS_DECORATIONS   =   2;
  static const uint32_t MWM_DECOR_ALL           =   1;
  decoration_hints.flags = MWM_HINTS_DECORATIONS;
  decoration_hints.decorations = decorated ? MWM_DECOR_ALL : 0;

  XChangeProperty(display, win,
                  motif_wm_hints,
                  motif_wm_hints, 32,
                  PropModeReplace,
                  (unsigned char*) &hints,
                  sizeof(decoration_hints) / sizeof(long));


  {
    XSizeHints* sizehints = XAllocSizeHints();
    if(!sizehints) {
      fprintf(stderr, "reif: cannot create X11 window because allocating WM hints failed.\n");
      return 0;
    }
    if(!resizable) {
      sizehints->flags |= (PMinSize | PMaxSize);
      sizehints->min_width  = sizehints->max_width  = width;
      sizehints->min_height = sizehints->max_height = height;
    }
    if (adjusting_pos) {
      sizehints->flags |= PPosition;
      sizehints->x = 0;
      sizehints->y = 0;
    }
    sizehints->flags |= PWinGravity;
    sizehints->win_gravity = StaticGravity;

    XSetWMNormalHints(display, win, sizehints);
    XFree(sizehints);
  }


  if (n_windows + 1 <= MAX_WINDOWS) {
    lf_win_register(win, glcontext, flags);
  } else {
    fprintf(stderr, "warning: reached maximum amount of windows to define callbacks for.\n");
  }

  return win;
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

  memset(cursors, 0, sizeof(cursors));

  init_pipe();
  return 0;
}


void 
lf_win_register(lf_window_t win, GLXContext glcontext, uint32_t flags) {
    window_callbacks[n_windows].win = win;
    int32_t w, h;
  get_window_size(display, win, &w, &h);
  window_callbacks[n_windows].win = win;
  window_callbacks[n_windows].win_width = w;
  window_callbacks[n_windows].win_height = h;
  window_callbacks[n_windows].ev_mouse_press_cb = NULL;
  window_callbacks[n_windows].ev_mouse_release_cb = NULL;
  window_callbacks[n_windows].ev_close_cb = NULL;
  window_callbacks[n_windows].ev_refresh_cb = NULL;
  window_callbacks[n_windows].ev_resize_cb = NULL;
  window_callbacks[n_windows].ev_key_cb = NULL;
  window_callbacks[n_windows].ev_char_cb = NULL;
  window_callbacks[n_windows].ui = NULL;
  window_callbacks[n_windows].glcontext = glcontext;
  window_callbacks[n_windows].flags = flags;
  window_callbacks[n_windows].xinputcontext = XCreateIC(
    xim, XNInputStyle, 
    XIMPreeditNothing | XIMStatusNothing, 
    XNClientWindow, win, NULL);
  if (!window_callbacks[n_windows].xinputcontext) {
    fprintf(stderr, "reif: cannot create input context.\n");
  }
  ++n_windows;
}

int32_t 
lf_windowing_terminate(void) {
  XCloseDisplay(display);
  for(uint32_t i = 0; i < LF_CURSOR_COUNT; i++) {
    if(cursors[i] != 0) {
      XFreeCursor(display, cursors[i]);
    }
  }
  return 0;
}

void
lf_windowing_update(void) {
  current_event = LF_EVENT_NONE;
}

void 
lf_windowing_set_wait_events(bool wait) {
  wait_events = wait;
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


void lf_windowing_next_event(void) {
  XEvent event;
  int x11_fd = ConnectionNumber(display);

  // First: always check if events are already queued
  if (XPending(display)) {
    do {
      XNextEvent(display, &event);
      handle_event(&event);
      for (uint32_t i = 0; i < n_windows; i++) {
        if (window_callbacks[i].windowing_event_cb) {
          window_callbacks[i].windowing_event_cb(&event, window_callbacks[i].ui);
        }
      }
    } while (XPending(display));
    return;
  }

  if (false) {
    fd_set in_fds;
    FD_ZERO(&in_fds);
    FD_SET(x11_fd, &in_fds);
    FD_SET(pipe_fds[0], &in_fds);
    int max_fd = (x11_fd > pipe_fds[0] ? x11_fd : pipe_fds[0]) + 1;

    if (select(max_fd, &in_fds, NULL, NULL, NULL) > 0) {
      if (FD_ISSET(pipe_fds[0], &in_fds)) {
        drain_pipe();
      }

      if (FD_ISSET(x11_fd, &in_fds)) {
        XNextEvent(display, &event);
        handle_event(&event);
        for (uint32_t i = 0; i < n_windows; i++) {
          if (window_callbacks[i].windowing_event_cb) {
            window_callbacks[i].windowing_event_cb(&event, window_callbacks[i].ui);
          }
        }
        while (XPending(display)) {
          XNextEvent(display, &event);
          handle_event(&event);
          for (uint32_t i = 0; i < n_windows; i++) {
            if (window_callbacks[i].windowing_event_cb) {
              window_callbacks[i].windowing_event_cb(&event, window_callbacks[i].ui);
            }
          }
        }
      }
    }
  } else {
    while (XPending(display)) {
      XNextEvent(display, &event);
      handle_event(&event);
      for (uint32_t i = 0; i < n_windows; i++) {
        if (window_callbacks[i].windowing_event_cb) {
          window_callbacks[i].windowing_event_cb(&event, window_callbacks[i].ui);
        }
      }
    }
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
  if(!data) {
  return 1;
  }
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

void 
lf_win_set_cursor(lf_window_t win, lf_cursor_type_t cursor_type) {
  Cursor cursor = cursors[cursor_type];
  if(cursor == 0) {
    cursor = XCreateFontCursor(display, cursor_type);
    if(!cursor) return;
  }
  XDefineCursor(display, win, cursor);
  XFlush(display);
}

void 
lf_win_reset_cursor(lf_window_t win) {
  XUndefineCursor(display, win);
  XFlush(display);
}

#endif
