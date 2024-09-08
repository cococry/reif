#ifdef LF_X11
#include "../../include/leif/win.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
  
typedef struct {
  Display* dsp;
  GLXContext glcontext;
  Atom wm_delete_win;
  XIM xim;
} state_t;

static state_t s;

static int32_t create_gl_context();

int32_t
create_gl_context() {
  int screen = DefaultScreen(s.dsp);

  // Choose the appropriate visual
  int attribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
  XVisualInfo* visual = glXChooseVisual(s.dsp, screen, attribs);
  if (!visual) {
    fprintf(stderr, "txt: no suitable OpenGL visual found.");
    return 1;
  }

  // Create OpenGL context
  s.glcontext = glXCreateContext(s.dsp, visual, NULL, GL_TRUE);
  if (!s.glcontext) {
    fprintf(stderr, "txt: cannot create OpenGL context.");
    return 1;
  }

  return 0;
}

int32_t
lf_windowing_init() {
  s.dsp = XOpenDisplay(NULL);

  if(!s.dsp) {
    fprintf(stderr, "txt: cannot open X display.");
    return 1;
  }
  
  s.wm_delete_win = XInternAtom(s.dsp, "WM_DELETE_WINDOW", True);
  if(s.wm_delete_win == None) {
    fprintf(stderr, "txt: cannot get WM_DELETE_WINDOW atom.");
    return 1;
  }

  XSetLocaleModifiers("");
  s.xim = XOpenIM(s.dsp, NULL, NULL, NULL);
  if (s.xim == NULL) {
    fprintf(stderr, "txt: cannot open X input method\n");
    return 1;
  }

  if(create_gl_context() != 0) {
    return 1;
  }

  return 0;
}

int32_t
lf_windowing_terminate() {
  if(XCloseDisplay(s.dsp) != 0) {
    fprintf(stderr, "txt: closing X display failed.");
    return 1;
  }
  XCloseIM(s.xim);
  return 0;
}

Display* win_get_display() {
  return s.dsp;
}

lf_window_t*
lf_win_create(uint32_t width, uint32_t height) {
  lf_window_t* win = malloc(sizeof(*win));

  win->win = XCreateSimpleWindow(s.dsp, DefaultRootWindow(s.dsp),
      0, 0, width, height,
      0, 0, 0); 

  XMapWindow(s.dsp, win->win);

  win->width = width;
  win->height = height;
  win->title = NULL;

  XSelectInput(s.dsp, win->win,
               ExposureMask | 
               KeyPressMask | 
               KeyReleaseMask |
               ButtonPressMask | 
               ButtonReleaseMask |
               PointerMotionMask); 

  XSetWMProtocols(s.dsp, win->win, &s.wm_delete_win, 1);

  win->xinputcontext = 
    XCreateIC(s.xim, 
              XNInputStyle, 
              XIMPreeditNothing |
              XIMStatusNothing, 
              XNClientWindow, 
              win->win, NULL);

  if (!win->xinputcontext) {
    fprintf(stderr, "leif: cannot create input context\n");
    return NULL;
  }

  XFlush(s.dsp);

  return win;
}

void 
lf_win_set_title(lf_window_t* win, const char* title) {
  if(!title || !win) return;

  win->title = strdup(title);
  XStoreName(s.dsp, win->win, title);
  XFlush(s.dsp);
}

int32_t
lf_win_make_gl_context(lf_window_t* win) {
  GLXDrawable drawable = (GLXDrawable)win->win;
  if (!glXMakeCurrent(s.dsp, drawable, s.glcontext)) {
    fprintf(stderr, "txt: unable to make GLX context current.");
    return 1;
  }
  return 0;
}

void lf_win_swap_buffers(lf_window_t* win) {
  glXSwapBuffers(s.dsp, win->win);
}

lf_event_t 
lf_win_next_event(lf_window_t* win) {
  lf_event_t ev;
  memset(&ev, 0, sizeof(ev));

  XEvent xev;
  XNextEvent(s.dsp, &xev);
  if(xev.type == Expose) {
    ev.type = WinEventExpose;
    ev.width = xev.xexpose.width; 
    ev.height = xev.xexpose.height;
    win->width = ev.width; 
    win->height = ev.height; 
    return ev;
  }
  if (xev.type == ClientMessage) {
    if ((Atom)xev.xclient.data.l[0] == s.wm_delete_win) {
      ev.type = WinEventClose;
    }
    return ev;
  }
  if(xev.type == KeyPress) {
    ev.type = WinEventKeyPress;
    ev.keycode = xev.xkey.keycode;
    ev.keystate = xev.xkey.state;

    KeySym keysym;
    char buf[32];
    Status status;
    int len = Xutf8LookupString(win->xinputcontext, 
                                &xev.xkey, 
                                buf, sizeof(buf) - 1,
                                &keysym, &status);
    buf[len] = '\0';  

    if(len > 0) {
      strcpy(ev.keyunicode, buf);
    } else {
      memset(ev.keyunicode, 0, sizeof(ev.keyunicode));
    }

    return ev;
  }
  if(xev.type == KeyRelease) {
    ev.type = WinEventKeyRelease;
    ev.keycode = xev.xkey.keycode;
    ev.keystate = xev.xkey.state;
    return ev;
  }

  if(xev.type == MotionNotify) {
    ev.type = WinEventMouseMove;
    ev.x = xev.xmotion.x;
    ev.y = xev.xmotion.y;
    return ev;
  }
  
  if(xev.type == ButtonPress) {
    ev.type = WinEventMousePress;
    ev.button = xev.xbutton.button;
    ev.x = xev.xbutton.x;
    ev.y = xev.xbutton.y;
    return ev;
  }

  XFlush(s.dsp);
  return ev;
}

void 
lf_win_destroy(lf_window_t* win) {
  XUnmapWindow(s.dsp, win->win);
  XDestroyWindow(s.dsp, win->win);
  XFlush(s.dsp);
  XDestroyIC(win->xinputcontext);

  if(win->title) {
    free(win->title);
    win->title = NULL;
  }

  free(win);
}

uint32_t 
lf_win_get_keycode(KeySym keysym) {
  return XKeysymToKeycode(s.dsp, keysym); 
}
#else
#error "Invalid windowing system specified (valid windowing systems: LF_X11)"
#endif
