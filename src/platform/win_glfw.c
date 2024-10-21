#include <stdint.h>
#ifdef LF_GLFW
#include "../../include/leif/win.h"
#include "../../include/leif/ui_core.h"
#include "../../include/leif/event.h"
#include "../../include/leif/widget.h"
#include <stdlib.h>
#include <stdio.h>

#include <GLFW/glfw3.h>

#define MAX_WINDOWS 16

typedef struct {
  lf_win_mouse_press_func ev_mouse_press_cb;
  lf_win_mouse_release_func ev_mouse_release_cb;
  lf_win_refresh_func ev_refresh_cb;
  lf_win_resize_func ev_resize_cb;
  lf_win_close_func ev_close_cb;
  lf_win_mouse_move_func ev_move_cb;
  GLFWwindow* win;
} window_callbacks_t; 

static window_callbacks_t window_callbacks[MAX_WINDOWS];
static uint32_t n_windows = 0;
static lf_ui_state_t* ui;
static lf_event_type_t current_event;

static void glfw_mouse_button_callback(
  GLFWwindow* window, 
  int32_t button, 
  int32_t action, 
  int32_t mods); 

static void glfw_resize_callback(
  GLFWwindow* window,
  int32_t w, int32_t h);

static void glfw_close_callback(
  GLFWwindow* window);

static void glfw_mouse_move_callback(
  GLFWwindow* window, double xpos, double ypos);

void 
glfw_mouse_button_callback(
  GLFWwindow* window, 
  int32_t button, 
  int32_t action, 
  int32_t mods) {
  (void)window;
  (void)mods;
  lf_event_t ev;
  ev.button = button;
  ev.type = (action != GLFW_RELEASE) ? WinEventMousePress : WinEventMouseRelease;

  double x, y;
  glfwGetCursorPos(window, &x, &y);
  ev.x = (int16_t)x; 
  ev.y = (int16_t)y; 
  lf_widget_handle_event(ui, ui->root, ev);
  
  if(action != GLFW_RELEASE) {
    for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_mouse_press_cb)
        window_callbacks[i].ev_mouse_press_cb(ui, window, button);
    }
    current_event = WinEventMouseRelease;
  } else {
    for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_mouse_release_cb)
        window_callbacks[i].ev_mouse_release_cb(ui, window, button);
    }
    current_event = WinEventMousePress;
  }

}


void 
glfw_resize_callback(
  GLFWwindow* window,
  int32_t w, int32_t h) {
  lf_event_t ev;
  ev.type = WinEventResize; 
  ev.width = w; ev.height = h;
  current_event = WinEventResize;
  lf_widget_handle_event(ui, ui->root, ev);
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_resize_cb)
        window_callbacks[i].ev_resize_cb(ui, window, w, h);
  }
}

void 
glfw_refresh_callback(
  GLFWwindow* window) {
  lf_event_t ev;
  ev.type = WinEventRefresh; 
  current_event = WinEventRefresh;
  lf_widget_handle_event(ui, ui->root, ev);
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_resize_cb)
        window_callbacks[i].ev_refresh_cb(ui, window);
  }
}

void 
glfw_close_callback(GLFWwindow* window) {
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_close_cb)
        window_callbacks[i].ev_close_cb(ui, window);
    }
  lf_event_t ev;
  ev.type = WinEventClose; 
  current_event = WinEventClose;
  lf_widget_handle_event(ui, ui->root, ev);
}

void 
glfw_mouse_move_callback(
  GLFWwindow* window, double xpos, double ypos) {
  for(uint32_t i = 0; i < n_windows; i++) {
      if(window_callbacks[i].win == window && window_callbacks[i].ev_move_cb)
        window_callbacks[i].ev_move_cb(ui, window, (uint16_t)xpos, (uint16_t)ypos);
  }

  lf_event_t ev;
  ev.x = (uint16_t)xpos;
  ev.y = (uint16_t)ypos;
  ev.type = WinEventMouseMove;
  current_event = WinEventMouseMove; 
  lf_widget_handle_event(ui, ui->root, ev);
}

int32_t 
lf_windowing_init(void) {
  if(!glfwInit()) {
    fprintf(stderr, "reif: cannot initialize GLFW windowing system.\n");
    return 1;
  }
  return 0;
}

int32_t 
lf_windowing_terminate(void) {
  glfwTerminate();
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
  glfwPollEvents();
}

void*
lf_win_get_display(void) {
  return NULL;
}

lf_window_t* 
lf_win_create(uint32_t width, uint32_t height, const char* title) {

  GLFWwindow* win = glfwCreateWindow(
    width, height, 
    title,
    NULL, NULL);

  if(n_windows + 1 <= MAX_WINDOWS) {
    window_callbacks[n_windows].win = win;
    window_callbacks[n_windows].ev_mouse_press_cb = NULL;
    window_callbacks[n_windows].ev_mouse_release_cb = NULL;
    window_callbacks[n_windows].ev_close_cb = NULL;
    window_callbacks[n_windows].ev_refresh_cb = NULL;
    ++n_windows;
    glfwSetMouseButtonCallback(win, glfw_mouse_button_callback);
    glfwSetWindowCloseCallback(win, glfw_close_callback);
    glfwSetFramebufferSizeCallback(win, glfw_resize_callback);
    glfwSetWindowRefreshCallback(win, glfw_refresh_callback);
    glfwSetCursorPosCallback(win, glfw_mouse_move_callback);
  }
  else {
    fprintf(stderr, "warning: reached maximum amount of windows to define callbacks for.\n");
  }

  glfwSwapInterval(1);

  return win;
}

void 
lf_win_set_title(lf_window_t* win, const char* title) {
  glfwSetWindowTitle(win, title); 
}

int32_t 
lf_win_make_gl_context(lf_window_t* win) {
  glfwMakeContextCurrent(win);
  return 0;
}

void 
lf_win_swap_buffers(lf_window_t* win) {
  glfwSwapBuffers(win);
}

void 
lf_win_destroy(lf_window_t* win) {
  glfwDestroyWindow(win);
  glfwPostEmptyEvent();
}

vec2s 
lf_win_cursor_pos(lf_window_t* win) {
  double x, y;
  glfwGetCursorPos(win, &x, &y);
  vec2s cursor = (vec2s){
    .x = (float)x,
    .y = (float)y};
  return cursor;
}

void 
lf_win_set_close_cb(lf_window_t* win, lf_win_close_func close_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == win) {
      window_callbacks[i].ev_close_cb = close_cb;
      break;
    }
  }
}

void 
lf_win_set_refresh_cb(lf_window_t* win, lf_win_refresh_func refresh_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == win) {
      window_callbacks[i].ev_refresh_cb = refresh_cb;
      break;
    }
  }
}

void 
lf_win_set_resize_cb(lf_window_t* win, lf_win_resize_func resize_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == win) {
      window_callbacks[i].ev_resize_cb = resize_cb;
      break;
    }
  }
}

void 
lf_win_set_mouse_press_cb(lf_window_t* win, lf_win_mouse_press_func mouse_press_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == win) {
      window_callbacks[i].ev_mouse_press_cb = mouse_press_cb;
      break;
    }
  }
}

void 
lf_win_set_mouse_release_cb(lf_window_t* win, lf_win_mouse_release_func mouse_release_cb) {
  for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == win) {
      window_callbacks[i].ev_mouse_release_cb = mouse_release_cb;
      break;
    }
  }
}

void 
lf_win_set_mouse_move_cb(lf_window_t* win, lf_win_mouse_move_func mouse_move_cb) {
 for (uint32_t i = 0; i < n_windows; ++i) {
    if (window_callbacks[i].win == win) {
      window_callbacks[i].ev_move_cb = mouse_move_cb;
      break;
    }
  }
}
vec2s 
lf_win_get_size(lf_window_t* win) {
  int32_t width, height;
  glfwGetWindowSize(win, &width, &height);
  return (vec2s){
    .x = (float)width,
    .y = (float)height,
  };
}

int32_t 
lf_win_get_refresh_rate(lf_window_t* win) {
    int window_x, window_y;
    glfwGetWindowPos(win, &window_x, &window_y);

    int monitor_count;
    GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
    for (int i = 0; i < monitor_count; i++) {
        int monitor_x, monitor_y;
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

        glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);

        if (window_x >= monitor_x && window_x < monitor_x + mode->width &&
            window_y >= monitor_y && window_y < monitor_y + mode->height) {
            return mode->refreshRate; 
        }
    }

    return -1;
}
#endif
